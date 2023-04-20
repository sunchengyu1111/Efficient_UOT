#include "MAP-UOT_Testprint_0.h"

void printMV(double *uot, double *distri_row, double *distri_col, double *sum_col0, double *sum_col1, double *sum_col2, const int m, const int n) {
        printf("\n-----uot\n");
        for(int i = 0; i < m; ++i) {
                for(int j = 0; j < n; ++j) {
                        printf("%f ", uot[i * n + j]);
                }
                printf("\n");
        }
        printf("\n-----distri_row\n");
        for(int i = 0; i < m; ++i) {
                printf("%f ", distri_row[i]);
        }
        printf("\n-----distri_col\n");
        for(int i = 0; i < n; ++i) {
                printf("%f ", distri_col[i]);
        }
        printf("\n-----sum_col0\n");
        for(int i = 0; i < n; ++i) {
                printf("%f ", sum_col0[i]);
        }
        printf("\n-----sum_col1\n");
        for(int i = 0; i < n; ++i) {
                printf("%f ", sum_col1[i]);
        }
        printf("\n-----sum_col2\n");
        for(int i = 0; i < n; ++i) {
                printf("%f ", sum_col2[i]);
        }
}
