#ifndef _MAPUOT_TESTMPIMAP_H
#define _MAPUOT_TESTMPIMAP_H

#include <stdio.h>
#include <math.h>
#include <cstring>
#include <mpi.h>
#include "MAP-UOT_parameter.h"
#include "MAP-UOT_Testprint_0.h"

void UOT_MPIMAP(double *uot, double *distri_row, double *distri_col, double *sum_col0, double *sum_col1, double *sum_col2, const int m, const int n, const int w, const double fi, double error_0, int count_0, int world_rank);
static double UOT_iter(double *uot, double *distri_row, double *distri_col, double *sum_col0, double *sum_col1, double *sum_col2, const int m, const int n, const double fi);

#endif
