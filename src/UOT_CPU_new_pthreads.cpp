#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <immintrin.h>
#include <sys/time.h>
#define eps 1e-1

int thread_count;

double cpuSecond(){
        struct timeval tp;
        gettimeofday(&tp,NULL);
        return ((double)tp.tv_sec+(double)tp.tv_usec*1e-6);
}

struct arg_struct{
	double *uot;
	double *distri_row;
	double *distri_col;
	double *sum_col0;
	double *sum_col1;
	double *sum_col2;
	int m;
	int n;
	double fi;
	int thread;
};

void printMV(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,double *sum_col2,const int m,const int n);

void UOT_typical(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,const int m,const int n,const double fi);

void* UOT_iter(void* arg_struct);

int main(int argc,char *argv[]){
	int thread;
	pthread_t *thread_handles;
	thread_count=atoi(argv[1]);
	thread_handles=(pthread_t*)malloc(thread_count*sizeof(pthread_t));
	int m=atoi(argv[2]);
	int n=atoi(argv[3]);

	//test new
	double *uot_0=new double[m*n];
	double *distri_row_0=new double[m];
	double *distri_col_0=new double[n];
	double *sum_col0_0=new double[n];
	double *sum_col0_1=new double[n*thread_count];
	double *sum_col0_2=new double[n];
	for(int i=0;i<m*n;i++){
		uot_0[i]=(double)rand()/(double)(RAND_MAX)*100;                      
	}
	for(int i=0;i<m;i++){
		distri_row_0[i]=(double)rand()/(double)(RAND_MAX)*100;
	}
	for(int i=0;i<n;i++){
		distri_col_0[i]=(double)rand()/(double)(RAND_MAX)*100;
	}
	int count_0=0;
	double error_0=1;
	double fi=0.5;
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			sum_col0_0[i]+=uot_0[i+j*n];
		}	
	}

	//test typical
        double *uot_1=new double[m*n];
        double *distri_row_1=new double[m];
        double *distri_col_1=new double[n];
        double *sum_col1_0=new double[n];
        double *sum_col1_1=new double[n];
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
	//printMV(uot_0,distri_row_0,distri_col_0,sum_col0_0,sum_col0_1,sum_col0_2,m,n);
	double start_0=cpuSecond();
	while(error_0>=eps){
		error_0=0;
		//printf("count_0=%d.\n",count_0);
		memset(sum_col0_1,0,n*thread_count*sizeof(double));
		for(int i=0;i<n;i++){
			sum_col0_2[i]=sum_col0_0[i];
		}
		for(int i=0;i<n;i++){
			sum_col0_0[i]=pow(distri_col_0[i]/sum_col0_0[i],fi);
		}
		struct arg_struct args_0[thread_count];
		double start_1=cpuSecond();
		for(thread=0;thread<thread_count;thread++){
			args_0[thread].uot=uot_0;
			args_0[thread].distri_row=distri_row_0;
			args_0[thread].distri_col=distri_col_0;
			args_0[thread].sum_col0=sum_col0_0;
			args_0[thread].sum_col1=sum_col0_1;
			args_0[thread].sum_col2=sum_col0_2;
			args_0[thread].m=m;
			args_0[thread].n=n;
			args_0[thread].fi=fi;
			args_0[thread].thread=thread;
			pthread_create(&thread_handles[thread],NULL,UOT_iter,(void*) &args_0[thread]);		
		}
		for(thread=0;thread<thread_count;thread++){
			pthread_join(thread_handles[thread],NULL);
		}
		double finish_1=cpuSecond()-start_1;
		//printf("time_1=%f.\n",finish_1*1000);
		memset(sum_col0_0,0,n*sizeof(double));
		for(int i=0;i<thread_count;i++){
			for(int j=0;j<n;j++){
				sum_col0_0[j]+=sum_col0_1[i*n+j];
			}
		}
		for(int i=0;i<n;i++){
			error_0+=fabs(sum_col0_0[i]-sum_col0_2[i]);
		}
		//printf("error_0=%f.\n",error_0);
		//printMV(uot_0,distri_row_0,distri_col_0,sum_col0_0,sum_col0_1,sum_col0_2,m,n);
		count_0++;
	}
	double finish_0=cpuSecond()-start_0;
	//double x=double(m)*double(n)*12/1e9;
        //printf("%.3f,",1000/(finish_0*1000/count_0)*x);
        printf("new_time=%f,",finish_0*1000/count_0);
	//printMV(uot_0,distri_row_0,distri_col_0,sum_col0_0,sum_col0_1,sum_col0_2,m,n);
	free(thread_handles);

	//compute error
	double error=0;
	for(int i=0;i<m*n;i++){
		error+=abs(uot_0[i]-uot_1[i]);
	}
	//printf("\nerror=%f.\n",error);
	
	return 0;
}

void printMV(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,double *sum_col2,const int m,const int n){
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
        printf("\n-----sum_col0\n");
        for(int i=0;i<n;i++){
                printf("%f ",sum_col0[i]);
        }
        printf("\n-----sum_col1\n");
        for(int i=0;i<n;i++){
                printf("%f ",sum_col1[i]);
        }
        printf("\n-----sum_col2\n");
        for(int i=0;i<n;i++){
                printf("%f ",sum_col2[i]);
        }
}

void UOT_typical(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,const int m,const int n,const double fi){
	int count_1=0;
	double error_1=1;
	//pre col sum
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			sum_col0[i]+=uot[i+j*n];
		}	
	}
	clock_t start_1=clock();
	while(error_1>=eps){
		//printMV(uot,distri_row,distri_col,sum_col0,sum_col1,m,n);
		//printf("count_1=%d.\n",count_1);
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
			double temp=0;
			for(int j=0;j<n;j++){
				temp+=uot[i*n+j];
			}
			temp=pow(distri_row[i]/temp,fi);
			for(int j=0;j<n;j++){
				uot[i*n+j]*=temp;
			}
		}
		//col sum
		memset(sum_col0,0,n*sizeof(double));
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
		//printf("error_1=%f\n",error_1);
	}
	clock_t finish_1=clock();
	//printf("typical_time=%f\n",(double)(finish_1-start_1)/CLOCKS_PER_SEC*1000/count_1);
}

void* UOT_iter(void* arguments){
	struct arg_struct *args=(struct arg_struct*) arguments;
	double *uot=args->uot;
	double *distri_row=args->distri_row;
        double *distri_col=args->distri_col;
        double *sum_col0=args->sum_col0;
        double *sum_col1=args->sum_col1;
        double *sum_col2=args->sum_col2;
        int m=args->m;
        int n=args->n;
	int rank=args->thread;
        double fi=args->fi;
	int local_m=m/thread_count;
	int start_row=rank*local_m;
	int finish_row=(rank+1)*local_m;
	double *p_uot=uot+start_row*n;
	double *p_sum_col0=sum_col0;
	double *p_sum_col1=sum_col1+n*rank+n-1;
	for(int i=start_row;i<finish_row;i++){
		double sum_row=0;
		for(int j=0;j<n;j++,p_uot++,p_sum_col0++){
			*p_uot*=*p_sum_col0;
			sum_row+=*p_uot;
		}
		*p_uot--;
		sum_row=pow(distri_row[i]/sum_row,fi);
		for(int j=0;j<n;j++,p_uot--,p_sum_col1--){
			*p_uot*=sum_row;
			*p_sum_col1+=*p_uot;
		}
		p_uot+=n+1;
		p_sum_col0-=n;
		p_sum_col1+=n;
	}
	return NULL;
}
