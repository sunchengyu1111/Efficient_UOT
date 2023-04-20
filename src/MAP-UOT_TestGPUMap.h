#ifndef _MAPUOT_TESTGPUMAP_H
#define _MAPUOT_TESTGPUMAP_H

#include <stdio.h>
#include <math.h>
#include <cstring>
#include <cuda.h>
#include <cuda_runtime.h>
#include "MAP-UOT_parameter.h"
#include "MAP-UOT_Testprint_1.h"

double cpuSecond(void);
void UOT_GPUMAP(float *uot_0, float *distri_row_0, float *distri_col_0, float *sum_row0_0, float *sum_col0_0, float *sum_col0_1, float error_0, const int m, const int n, const double fi, int count_0);
__global__ void modify_row_factor(float *distri_row, float *sum_row, float fi);
__global__ void modify_col_factor(float *distri_col, float *sum_col, float fi);
__global__ void UOT_error(float *sum_col0, float *sum_col1, float *error);
__global__ void UOT_iterbase_rowsum(float *uot, float *sum_row, float *sum_col, int m, int n, int iter_y);
__global__ void UOT_iterbase_colsum(float *uot, float *sum_row, float *sum_col, int m, int n, int iter_y);

#endif
