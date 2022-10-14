#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <sys/time.h>
#define CHECK(call){									\
	const cudaError_t error=call; 							\
	if(error!=cudaSuccess){								\
		printf("Error:%s.%d.\n",__FILE__,__LINE__);				\
		printf("Code:%d.Reason:%s.\n",error,cudaGetErrorString(error));		\
		exit(-10*error);							\
	}										\
}
#define eps 1e-1

double cpuSecond(){
	struct timeval tp;
	gettimeofday(&tp,NULL);
	return ((double)tp.tv_sec+(double)tp.tv_usec*1e-6);
}

void printMV(float *uot,float *distri_row,float *distri_col,float *sum_row0,float *sum_col0,float *sum_col1,float error,const int m,const int n);

void UOT_typical(float *uot,float *distri_row,float *distri_col,float *sum_col0,float *sum_col1,const int m,const int n,const float fi);

__global__ void modify_row_factor(float *distri_row,float *sum_row,float fi);

__global__ void modify_col_factor(float *distri_col,float *sum_col,float fi);

__global__ void UOT_error(float *sum_col0,float *sum_col1,float *error);

__global__ void UOT_iterbase_rowsum(float *uot,float *sum_row,float *sum_col,int m,int n,int iter_y);

__global__ void UOT_iterbase_colsum(float *uot,float *sum_row,float *sum_col,int m,int n,int iter_y);

int main(int argc,char **argv){
	int m=atoi(argv[1]);
	int n=atoi(argv[2]);

	//test new
	float *uot_0=new float[m*n];
	float *distri_row_0=new float[m];
	float *distri_col_0=new float[n];
	float *sum_row0_0=new float[m];
	float *sum_col0_0=new float[n];
	float *sum_col0_1=new float[n];
	for(int i=0;i<m*n;i++){
		uot_0[i]=(float)rand()/(float)(RAND_MAX)*100;
	}
	for(int i=0;i<m;i++){
		distri_row_0[i]=(float)rand()/(float)(RAND_MAX)*100;
	}
	for(int i=0;i<n;i++){
		distri_col_0[i]=(float)rand()/(float)(RAND_MAX)*100;
	}
	int count_0=0;
	float error_0=1;
	float fi=0.5;
	double start=0,finish=0;//,start0,finish0,start1,finish1;
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			sum_col0_0[i]+=uot_0[i+j*n];
		}
	}
	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			sum_row0_0[i]+=uot_0[i*n+j];
		}
	}
	//printMV(uot_0,distri_row_0,distri_col_0,sum_row0_0,sum_col0_0,sum_col0_1,error_0,m,n);
		
	//test typical
	float *uot_1=new float[m*n];
	float *distri_row_1=new float[m];
	float *distri_col_1=new float[n];
	float *sum_col1_0=new float[n];
	float *sum_col1_1=new float[n];
	for(int i=0;i<m*n;i++){
		uot_1[i]=uot_0[i];
	}
	for(int i=0;i<m;i++){
		distri_row_1[i]=distri_row_0[i];
	}
	for(int i=0;i<n;i++){
		distri_col_1[i]=distri_col_0[i];
	}
	UOT_typical(uot_1,distri_row_1,distri_col_1,sum_col1_0,sum_col1_1,m,n,fi);
	
	//test new
	float *d_uot_0,*d_distri_row_0,*d_distri_col_0,*d_sum_row0_0,*d_sum_col0_0,*d_sum_col0_1,*d_error_0;
	cudaMalloc((float**)&d_uot_0,m*n*sizeof(float));
	cudaMalloc((float**)&d_distri_row_0,m*sizeof(float));
	cudaMalloc((float**)&d_distri_col_0,n*sizeof(float));
	cudaMalloc((float**)&d_sum_row0_0,m*sizeof(float));
	cudaMalloc((float**)&d_sum_col0_0,n*sizeof(float));
	cudaMalloc((float**)&d_sum_col0_1,n*sizeof(float));
	cudaMalloc((float**)&d_error_0,sizeof(float));
	cudaMemcpy(d_uot_0,uot_0,m*n*sizeof(float),cudaMemcpyHostToDevice);
	cudaMemcpy(d_distri_row_0,distri_row_0,m*sizeof(float),cudaMemcpyHostToDevice);
	cudaMemcpy(d_distri_col_0,distri_col_0,n*sizeof(float),cudaMemcpyHostToDevice);
	cudaMemcpy(d_sum_row0_0,sum_row0_0,m*sizeof(float),cudaMemcpyHostToDevice);
	cudaMemcpy(d_sum_col0_0,sum_col0_0,n*sizeof(float),cudaMemcpyHostToDevice);
	cudaMemcpy(d_sum_col0_1,sum_col0_1,n*sizeof(float),cudaMemcpyHostToDevice);
	cudaMemcpy(d_error_0,&error_0,sizeof(float),cudaMemcpyHostToDevice);
	//parameter
	const int temp=128;
	const int c_block_x=128;
	const int c_iter_y=8;
	dim3 c_block(c_block_x);
	//add '/4'
	dim3 c_grid((n+c_block_x-1)/c_block_x,(m+c_iter_y-1)/c_iter_y);
	const int r_block_x=128;
	const int r_block_y=2;
	const int r_iter_y=8;
	dim3 r_block(r_block_x,r_block_y);
	//add '/4'
	dim3 r_grid((n+r_block_x-1)/r_block_x,(m+r_block_y*r_iter_y-1)/(r_block_y*r_iter_y));
	double time=0;//,time0=0,time1=0;
	while(error_0>=eps){
		//printf("count_0=%d\n",count_0);
		count_0++;
		start=cpuSecond();
		cudaMemset(d_error_0,0,sizeof(float));
		cudaMemcpy(d_sum_col0_1,d_sum_col0_0,n*sizeof(float),cudaMemcpyDeviceToDevice);	
		modify_col_factor<<<(n+temp-1)/temp,temp>>>(d_distri_col_0,d_sum_col0_0,fi);
		cudaMemset(d_sum_row0_0,0,m*sizeof(float));
		UOT_iterbase_colsum<<<c_grid,c_block>>>(d_uot_0,d_sum_row0_0,d_sum_col0_0,m,n,c_iter_y);
		modify_row_factor<<<(n+temp-1)/temp,temp>>>(d_distri_row_0,d_sum_row0_0,fi);
		cudaMemset(d_sum_col0_0,0,n*sizeof(float));
		UOT_iterbase_rowsum<<<r_grid,r_block>>>(d_uot_0,d_sum_row0_0,d_sum_col0_0,m,n,r_iter_y);
		UOT_error<<<(n+temp-1)/temp,temp>>>(d_sum_col0_0,d_sum_col0_1,d_error_0);
		cudaDeviceSynchronize();
		finish=cpuSecond()-start;
		time+=finish;
		cudaMemcpy(sum_row0_0,d_sum_row0_0,m*sizeof(float),cudaMemcpyDeviceToHost);
		cudaMemcpy(sum_col0_0,d_sum_col0_0,n*sizeof(float),cudaMemcpyDeviceToHost);
		cudaMemcpy(sum_col0_1,d_sum_col0_1,n*sizeof(float),cudaMemcpyDeviceToHost);
		cudaMemcpy(uot_0,d_uot_0,m*n*sizeof(float),cudaMemcpyDeviceToHost);
		cudaMemcpy(&error_0,d_error_0,sizeof(float),cudaMemcpyDeviceToHost);
		//printf("error_0=%f\n",error_0);		
		//printMV(uot_0,distri_row_0,distri_col_0,sum_row0_0,sum_col0_0,sum_col0_1,error_0,m,n);
	}
	//double x=double(m)*double(n)*12/1e9;
	//printf("%f\t",1/(time/count_0)*x);
	printf("new_time=%f,",time/count_0);
	
	//compute error
	float error=0;
	for(int i=0;i<m*n;i++){
		error+=abs(uot_0[i]-uot_1[i]);
	}
	//printf("error=%f\n",error);
	
	return 0;
}

void printMV(float *uot,float *distri_row,float *distri_col,float *sum_row0,float *sum_col0,float *sum_col1,float error,const int m,const int n){
	printf("\n-----uot\n");
	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			printf("%f ",uot[i*n+j]);
		}
		printf("\n");
	}
	printf("\n-----distri_row\n");
	for(int i=0;i<m;i++){
		printf("%f ",distri_row[i]);
	}
	printf("\n-----distri_col\n");
	for(int i=0;i<n;i++){
		printf("%f ",distri_col[i]);
	}
	printf("\n-----sum_row0\n");
	for(int i=0;i<m;i++){
		printf("%f ",sum_row0[i]);
	}
	printf("\n-----sum_col0\n");
	for(int i=0;i<n;i++){
		printf("%f ",sum_col0[i]);
	}
	printf("\n-----sum_col1\n");
	for(int i=0;i<n;i++){
		printf("%f ",sum_col1[i]);
	}
	printf("\n-----error\n");
	printf("%f",error);
}

void UOT_typical(float *uot,float *distri_row,float *distri_col,float *sum_col0,float *sum_col1,const int m,const int n,const float fi){
	int count_1=0;
	float error_1=1;
	//pre col sum
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			sum_col0[i]+=uot[i+j*n];
		}
	}
	clock_t start_1=clock();
	while(error_1>=eps){
		//printMV(uot,distri_row,distri_col,sum_col0,sum_col1,m,n);
		//printf("count_1=%d\n",count_1);
		count_1++;
		//copy
		for(int i=0;i<n;i++){
			sum_col1[i]=sum_col0[i];
		}
		//col 
		for(int i=0;i<n;i++){
			sum_col0[i]=pow(distri_col[i]/sum_col0[i],fi);
		}
		for(int i=0;i<n;i++){
			for(int j=0;j<m;j++){
				uot[i+j*n]*=sum_col0[i];
			}
		}
		//row
		for(int i=0;i<m;i++){
			float temp=0;
			for(int j=0;j<n;j++){
				temp+=uot[i*n+j];
			}
			temp=pow(distri_row[i]/temp,fi);
			for(int j=0;j<n;j++){
				uot[i*n+j]*=temp;
			}
		}
		//col sum
		memset(sum_col0,0,n*sizeof(float));
		for(int i=0;i<n;i++){
			for(int j=0;j<m;j++){
				sum_col0[i]+=uot[i+j*n];
			}
		}
		//compute error
		error_1=0;
		for(int i=0;i<n;i++){
			error_1+=fabs(sum_col0[i]-sum_col1[i]);
		}
		//printf("error_1=%f",error_1);
	}
	clock_t finish_1=clock();
	//printf("typical_time=%f\n",(float)(finish_1-start_1)/CLOCKS_PER_SEC*1000/count_1);
}

__global__ void modify_row_factor(float *distri_row,float *sum_row,float fi){
	int id=blockIdx.x*blockDim.x+threadIdx.x;
	sum_row[id]=powf(distri_row[id]/sum_row[id],fi);
}

__global__ void modify_col_factor(float *distri_col,float *sum_col,float fi){
	int id=blockIdx.x*blockDim.x+threadIdx.x;
	sum_col[id]=powf(distri_col[id]/sum_col[id],fi);
}

__global__ void UOT_error(float *sum_col0,float *sum_col1,float *error){
	int id=blockIdx.x*blockDim.x+threadIdx.x;
	int laneID=threadIdx.x%32;
	float t=abs(sum_col1[id]-sum_col0[id]);
	for(unsigned w=16;w>=1;w>>=1){
		t+=__shfl_down_sync(0xffffffff,t,w);
	}
	if(laneID==0) atomicAdd(error,t);
}

__global__ void UOT_iterbase_rowsum(float *uot,float *sum_row,float *sum_col,int m,int n,int iter_y){
	//int temp0=blockIdx.x+blockIdx.y+threadIdx.x+threadIdx.y;
	//if(temp0==0) printf("gridDim.x:%d,gridDim.y:%d,blockDim.x:%d,blockDim.y:%d\n",gridDim.x,gridDim.y,blockDim.x,blockDim.y);	
	//-----change along with blockDim.y-----//
	__shared__ float sm_col[2][128];
	__shared__ float sm_row[16];
	int col=blockDim.x*blockIdx.x+threadIdx.x;
	int row=iter_y*(blockDim.y*blockIdx.y+threadIdx.y);
	int idx=row*n+col;
	sm_col[threadIdx.y][threadIdx.x]=0;
	//-----change along with iter_y,blockDim.y-----//
	if(threadIdx.x<16&&threadIdx.y==0) sm_row[threadIdx.x]=sum_row[threadIdx.x+row];
	//printf("blockIdx.x:%d,blockIdx.y:%d,threadIdx.x:%d,threadIdx.y:%d,row:%d,col:%d,idx:%d,laneID:%d,offset:%d\n",blockIdx.x,blockIdx.y,threadIdx.x,threadIdx.y,row,col,idx,laneID,offset);
	__syncthreads();
	int sm_rowid=iter_y*threadIdx.y;
	#pragma unroll
	for(int i=0;i<iter_y;i++){
		float t=uot[idx];
		t*=sm_row[sm_rowid];
		sm_col[threadIdx.y][threadIdx.x]+=t;
		uot[idx]=t;
		sm_rowid++;
		idx+=n;
	}
	__syncthreads();
	//change along with blockDim.y
	if(threadIdx.y==0) atomicAdd(&sum_col[col],sm_col[0][threadIdx.x]+sm_col[1][threadIdx.x]);//+sm_col[2][laneID]+sm_col[3][laneID]);
}

__global__ void UOT_iterbase_colsum(float *uot,float *sum_row,float *sum_col,int m,int n,int iter_y){
	//int temp0=blockIdx.x+blockIdx.y+threadIdx.x+threadIdx.y;
	//if(temp0==0) printf("gridDim.x:%d,gridDim.y:%d,blockDim.x:%d,blockDim.y:%d\n",gridDim.x,gridDim.y,blockDim.x,blockDim.y);	
	int col=blockDim.x*blockIdx.x+threadIdx.x;
	int row=iter_y*blockIdx.y;
	int idx=row*n+col;
	int laneID=threadIdx.x%32;
	int offset=threadIdx.x/32;
	//-----change along with blockDim.x-----//
	__shared__ float sm_col0[128];
	__shared__ float sm_col1[4];
	//float *p_sum_col=&sum_col[col];
	sm_col0[threadIdx.x]=sum_col[col];
	//printf("blockIdx.x:%d,blockIdx.y:%d,threadIdx.x:%d,threadIdx.y:%d,row:%d,col:%d,idx:%d,laneID:%d,offset:%d\n",blockIdx.x,blockIdx.y,threadIdx.x,threadIdx.y,row,col,idx,laneID,offset);
	#pragma unroll
	for(int i=0;i<iter_y;i++){
		float t=uot[idx];
		t*=sm_col0[threadIdx.x];
		uot[idx]=t;
		for(unsigned w=16;w>=1;w>>=1){
			t+=__shfl_down_sync(0xffffffff,t,w);
		}
		if(laneID==0) sm_col1[offset]=t;
		__syncthreads();
		//-----change along with blockDim.x-----//
		if(threadIdx.x==0) atomicAdd(&sum_row[row],sm_col1[0]+sm_col1[1]+sm_col1[2]+sm_col1[3]);
		//if(threadIdx.x==1) atomicAdd(&sum_row[row],sm_col1[4]+sm_col1[5]+sm_col1[6]+sm_col1[7]);
		//if(threadIdx.x==2) atomicAdd(&sum_row[row],sm_col1[8]+sm_col1[9]+sm_col1[10]+sm_col1[11]);
		//if(threadIdx.x==3) atomicAdd(&sum_row[row],sm_col1[12]+sm_col1[13]+sm_col1[14]+sm_col1[15]);
		__syncthreads();
		row++;
		idx+=n;
	}
}
