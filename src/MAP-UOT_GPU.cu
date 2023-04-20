#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <sys/time.h>
#include "MAP-UOT_GPU.h"

int main(int argc, char **argv) {
	int m = atoi(argv[1]);
	int n = atoi(argv[2]);

	// data preprocess -- MAP-UOT
	float *uot_0 = new float[m * n];
	float *distri_row_0 = new float[m];
	float *distri_col_0 = new float[n];
	float *sum_row0_0 = new float[m];
	float *sum_col0_0 = new float[n];
	float *sum_col0_1 = new float[n];
	for(int i = 0; i < m * n; ++i) {
		uot_0[i] = (float)rand() / (float)(RAND_MAX) * 100;
	}
	for(int i = 0; i < m; ++i) {
		distri_row_0[i] = (float)rand() / (float)(RAND_MAX) * 100;
	}
	for(int i = 0; i < n; ++i) {
		distri_col_0[i] = (float)rand() / (float)(RAND_MAX) * 100;
	}
	int count_0 = 0;
	float error_0 = 1;
	float fi = 0.5;
	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < m; ++j) {
			sum_col0_0[i] += uot_0[i + j * n];
		}
	}
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			sum_row0_0[i] += uot_0[i * n + j];
		}
	}

	// data preprocess -- Typical-UOT
	float *uot_1 = new float[m * n];
	float *distri_row_1 = new float[m];
	float *distri_col_1 = new float[n];
	float *sum_col1_0 = new float[n];
	float *sum_col1_1 = new float[n];
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
	UOT_GPUMAP(uot_0, distri_row_0, distri_col_0, sum_row0_0, sum_col0_0, sum_col0_1, error_0, m, n, fi, count_0);	
	
	// compute error
	//compute_error(uot_0, uot_1, m, n);
	
	return 0;
}
