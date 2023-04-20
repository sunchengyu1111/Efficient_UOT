#include "MAP-UOT_TestMPIMap.h"

void UOT_MPIMAP(double *uot, double *distri_row, double *distri_col, double *sum_col0, double *sum_col1, double *sum_col2, const int m, const int n, const int w, const double fi, double error_0, int count_0, int world_rank) {
	bool loop = true;
	double time0, time1, start, finish;
	start = MPI_Wtime();
        while(loop) {
                //if(world_rank == 0) printf("count_0 = %d.\n", count_0);
                error_0 = UOT_iter(uot, distri_row, distri_col, sum_col0, sum_col1, sum_col2, m, n, fi);
                count_0++;
                if(error_0 < eps) loop = false;
        }
        finish = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
        time1 = (finish - start) * 1000;
        MPI_Allreduce(&time1, &time0, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        if(world_rank == 0) printf("%.3f, ", time0 / count_0 / w);
}

static double UOT_iter(double *uot, double *distri_row, double *distri_col, double *sum_col0, double *sum_col1, double *sum_col2, const int m, const int n, const double fi) {
        double error_0 = 0;
        double sum_row = 0;
        double temp = 0;
        double *p_uot = uot;
        double *p_sum_col0 = sum_col0;
        double *p_sum_col1 = sum_col1 + n - 1;
        memset(sum_col1, 0, n * sizeof(double));
        for(int i = 0; i < n; ++i) {
                sum_col2[i] = sum_col0[i];
        }
        for(int i = 0; i < n; ++i) {
                sum_col0[i] = pow(distri_col[i] / sum_col0[i], fi);
        }
        for(int i = 0; i < m; ++i) {
                for(int j = 0; j < n; ++j, ++p_uot, ++p_sum_col0) {
                        *p_uot *= *p_sum_col0;
                        sum_row += *p_uot;
                }
                *p_uot--;
                sum_row = pow(distri_row[i] / sum_row, fi);
                for(int j = 0; j < n; ++j, --p_uot, --p_sum_col1) {
                        *p_uot *= sum_row;
                        *p_sum_col1 += *p_uot;
                }
                p_uot += n+1;
                p_sum_col0 -= n;
                p_sum_col1 += n;
                sum_row = 0;
        }
        MPI_Allreduce(sum_col1, sum_col0, n, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        error_0 = 0;
        for(int i = 0; i < n; ++i) {
                error_0 += fabs(sum_col0[i] - sum_col2[i]);
        }
        //printf("error_0 = %f.\n", error_0);
        return error_0;
}
