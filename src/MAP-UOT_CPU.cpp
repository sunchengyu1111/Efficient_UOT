#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <immintrin.h>
#include <sys/time.h>
#include "MAP-UOT_CPU.h"

int main(int argc, char *argv[]) {
	int thread;
	pthread_t *thread_handles;
	thread_count = atoi(argv[1]);
	thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
	int m = atoi(argv[2]);
	int n = atoi(argv[3]);

	// data preprocess -- MAP-UOT
	double *uot_0 = new double[m * n];
	double *distri_row_0 = new double[m];
	double *distri_col_0 = new double[n];
	double *sum_col0_0 = new double[n];
	double *sum_col0_1 = new double[n * thread_count];
	double *sum_col0_2 = new double[n];
	for(int i = 0; i < m * n; ++i) {
		uot_0[i] = (double)rand() / (double)(RAND_MAX) * 100;                      
	}
	for(int i = 0; i < m; ++i) {
		distri_row_0[i] = (double)rand() / (double)(RAND_MAX) * 100;
	}
	for(int i = 0; i < n; ++i) {
		distri_col_0[i] = (double)rand() / (double)(RAND_MAX) * 100;
	}
	int count_0 = 0;
	double error_0 = 1;
	double fi = 0.5;
	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < m; ++j) {
			sum_col0_0[i] += uot_0[i + j * n];
		}	
	}

	// data preprocess -- Typical-UOT
        double *uot_1 = new double[m * n];
        double *distri_row_1 = new double[m];
        double *distri_col_1 = new double[n];
        double *sum_col1_0 = new double[n];
        double *sum_col1_1 = new double[n];
        for(int i = 0; i < m * n; ++i) {
                uot_1[i] = uot_0[i];
        }
        for(int i = 0; i < m; ++i) {
                distri_row_1[i] = distri_row_0[i];
        }
        for(int i = 0; i < n; ++i) {
                distri_col_1[i] = distri_col_0[i];
        }

	// test Typical-UOT
        //UOT_typical(uot_1, distri_row_1, distri_col_1, sum_col1_0, sum_col1_1, m, n, fi);

	// test MAP-UOT
	UOT_MAP(uot_0, distri_row_0, distri_col_0, sum_col0_0, sum_col0_1, sum_col0_2, thread, m, n, fi, count_0, error_0, thread_handles);

	// compute error
	//compute_error<double>(uot_0, uot_1, m, n);
	
	return 0;
}
