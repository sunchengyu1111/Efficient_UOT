#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define eps 1e-1

void printMV(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,const int m,const int n);

void UOT_typical(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,const int m,const int n,const double fi);

double UOT_iter(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,double *sum_col2,const int m,const int n,const double fi);

int main(int argc,char *argv[]){
	int m=atoi(argv[1]);
	int n=atoi(argv[2]);

	//test new
	double *uot_0=new double[m*n];
	double *distri_row_0=new double[m];
	double *distri_col_0=new double[n];
	double *sum_col0_0=new double[n];
	double *sum_col0_1=new double[n];
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
	clock_t start_0=clock();
	while(error_0>=eps){
		//printf("count_0=%d\n",count_0);
		if(count_0%2==0) error_0=UOT_iter(uot_0,distri_row_0,distri_col_0,sum_col0_0,sum_col0_1,sum_col0_2,m,n,fi);
		else error_0=UOT_iter(uot_0,distri_row_0,distri_col_0,sum_col0_1,sum_col0_0,sum_col0_2,m,n,fi);
		count_0++;
		//printf("error_0=%d\n",error_0);
	}
	clock_t finish_0=clock();
	//double x=double(m)*double(n)*12/1e9;
	//printf("%.3f,",1000/((double)(finish_0-start_0)/CLOCKS_PER_SEC*1000/count_0)*x);
	printf("new_time=%f,",(double)(finish_0-start_0)/CLOCKS_PER_SEC*1000/count_0);
	
	//compute error
	double error=0;
	for(int i=0;i<m*n;i++){
		error+=abs(uot_0[i]-uot_1[i]);
	}
	//printf("\nerror=%f\n",error);
	
}

void printMV(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,const int m,const int n){
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
		//printf("\nerror_1=%f",error_1);
	}
	clock_t finish_1=clock();
	//printf("typical_time=%f\n",(double)(finish_1-start_1)/CLOCKS_PER_SEC*1000/count_1);
}

double UOT_iter(double *uot,double *distri_row,double *distri_col,double *sum_col0,double *sum_col1,double *sum_col2,const int m,const int n,const double fi){
	double error_0=0;
	double sum_row=0;
	double temp=0;
	double *p_uot=uot;
	double *p_sum_col0=sum_col0;
	double *p_sum_col1=sum_col1+n-1;
	memset(sum_col1,0,n*sizeof(double));
	for(int i=0;i<n;i++){
		sum_col2[i]=sum_col0[i];
	}
	for(int i=0;i<n;i++){
		sum_col0[i]=pow(distri_col[i]/sum_col0[i],fi);
	}
	for(int i=0;i<m;i++){
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
		sum_row=0;
	}
	error_0=0;
	for(int i=0;i<n;i++){
		error_0+=fabs(sum_col1[i]-sum_col2[i]);
	}
	//printf("error_0=%f\n",error_0);
	return error_0;
}
