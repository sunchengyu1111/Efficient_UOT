#ifndef _MAPUOT_TESTMAP_H
#define _MAPUOT_TESTMAP_H

#include <stdio.h>
#include <math.h>
#include <cstring>
#include <pthread.h>
#include "MAP-UOT_parameter.h"
#include "MAP-UOT_Testprint_0.h"

extern int thread_count;
struct arg_struct {
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

double cpuSecond(void);
static void* UOT_iter(void* arguments);
void UOT_MAP(double *uot_0, double *distri_row_0, double *distri_col_0, double *sum_col0_0, double *sum_col0_1, double *sum_col0_2, int thread, const int m, const int n, const double fi, int count_0, double error_0, pthread_t *thread_handles);

#endif
