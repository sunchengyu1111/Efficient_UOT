#include "MAP-UOT_TestMPItypical.h"

void UOT_typical(double *uot, double *distri_row, double *distri_col, double *sum_col0, double *sum_col1, const int m, const int n, const double fi) {
        int count_1 = 0;
        double error_1 = 1;
        // pre col sum
        for(int i = 0; i < n; ++i) {
                for(int j = 0; j < m; ++j) {
                        sum_col0[i] += uot[i + j * n];
                }
        }
	double *sum_col2 = new double[n];
        MPI_Allreduce(sum_col0, sum_col2, n, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        clock_t start_1 = clock();
        while(error_1 >= eps) {
                //printMV(uot, distri_row, distri_col, sum_col0, sum_col1, m, n);
                //printf("count_1 = %d.\n", count_1);
                count_1++;
                // copy
                for(int i = 0; i < n; ++i) {
                        sum_col0[i]=sum_col2[i];
                        sum_col1[i]=sum_col2[i];
                }
                // col 
                for(int i = 0; i < n; ++i) {
                        sum_col0[i] = pow(distri_col[i] / sum_col0[i], fi);
                }
                for(int i = 0; i < n; ++i) {
                        for(int j = 0; j < m; ++j) {
                                uot[i + j * n] *= sum_col0[i];
                        }
                }
                // row
                for(int i = 0; i < m; ++i) {
                        double temp = 0;
                        for(int j = 0; j < n; ++j) {
                                temp += uot[i * n + j];
                        }
                        temp = pow(distri_row[i] / temp, fi);
                        for(int j = 0; j < n; ++j) {
                                uot[i * n + j] *= temp;
                        }
                }
                // col
		memset(sum_col0, 0, n * sizeof(double));
                for(int i = 0; i < n; ++i) {
                        for(int j = 0; j < m; ++j) {
                                sum_col0[i] += uot[i + j * n];
                        }
                }
		MPI_Allreduce(sum_col0,sum_col2,n,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
                // compute error
                error_1 = 0;
                for(int i = 0; i < n; ++i) {
                        error_1 += fabs(sum_col2[i] - sum_col1[i]);
                }
                //printf("error_1 = %f\n", error_1);
        }
        clock_t finish_1 = clock();
        //printf("typical_time = %f\n", (double)(finish_1 - start_1) / CLOCKS_PER_SEC * 1000 / count_1);
}
