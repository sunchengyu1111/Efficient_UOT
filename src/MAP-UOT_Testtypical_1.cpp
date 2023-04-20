#include "MAP-UOT_Testtypical_1.h"

void UOT_typical(float *uot, float *distri_row, float *distri_col, float *sum_col0, float *sum_col1, const int m, const int n, const float fi) {
        int count_1 = 0;
        float error_1 = 1;
        // pre col sum
        for(int i = 0; i < n; ++i) {
                for(int j = 0; j < m; ++j) {
                        sum_col0[i] += uot[i + j * n];
                }
        }
        clock_t start_1 = clock();
        while(error_1 >= eps) {
                //printf("count_1 = %d.\n", count_1);
                count_1++;
                // copy
                for(int i = 0; i < n; ++i) {
                        sum_col1[i] = sum_col0[i];
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
                        float temp = 0;
                        for(int j = 0; j < n; ++j) {
                                temp += uot[i * n + j];
                        }
                        temp = pow(distri_row[i] / temp, fi);
                        for(int j = 0; j < n; ++j) {
                                uot[i * n + j] *= temp;
                        }
                }
                // col
		memset(sum_col0, 0, n * sizeof(float));
                for(int i = 0; i < n; ++i) {
                        for(int j = 0; j < m; ++j) {
                                sum_col0[i] += uot[i + j * n];
                        }
                }
                // compute error
                error_1 = 0;
                for(int i = 0; i < n; ++i) {
                        error_1 += fabs(sum_col0[i] - sum_col1[i]);
                }
                //printf("error_1 = %f\n", error_1);
        }
        clock_t finish_1 = clock();
        //printf("typical_time = %f\n", (float)(finish_1 - start_1) / CLOCKS_PER_SEC * 1000 / count_1);
}
