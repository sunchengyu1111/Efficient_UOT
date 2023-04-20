#include "MAP-UOT_TestMap.h"

void UOT_MAP(double *uot_0, double *distri_row_0, double *distri_col_0, double *sum_col0_0, double *sum_col0_1, double *sum_col0_2, int thread, const int m, const int n, const double fi, int count_0, double error_0, pthread_t *thread_handles) {
	//printMV(uot_0, distri_row_0, distri_col_0, sum_col0_0, sum_col0_1, sum_col0_2, m, n);	
	double start_0 = cpuSecond();
        while(error_0 >= eps) {
                error_0 = 0;
                //printf("count_0 = %d.\n", count_0);
                memset(sum_col0_1, 0, n * thread_count * sizeof(double));
                for(int i = 0; i < n; ++i) {
                        sum_col0_2[i] = sum_col0_0[i];
                }
                for(int i = 0; i < n; ++i) {
                        sum_col0_0[i] = pow(distri_col_0[i] / sum_col0_0[i], fi);
                }
                struct arg_struct args_0[thread_count];
                //double start_1 = cpuSecond();
                for(thread = 0; thread < thread_count; ++thread) {
                        args_0[thread].uot = uot_0;
                        args_0[thread].distri_row = distri_row_0;
                        args_0[thread].distri_col = distri_col_0;
                        args_0[thread].sum_col0 = sum_col0_0;
                        args_0[thread].sum_col1 = sum_col0_1;
                        args_0[thread].sum_col2 = sum_col0_2;
                        args_0[thread].m = m;
                        args_0[thread].n = n;
                        args_0[thread].fi = fi;
                        args_0[thread].thread = thread;
                        pthread_create(&thread_handles[thread], NULL, UOT_iter, (void*) &args_0[thread]);
                }
                for(thread = 0; thread < thread_count; ++thread) {
                        pthread_join(thread_handles[thread], NULL);
                }
                //double finish_1 = cpuSecond() - start_1;
                //printf("time_1 = %f.\n", finish_1*1000);
                memset(sum_col0_0, 0, n * sizeof(double));
                for(int i = 0; i < thread_count; ++i) {
                        for(int j = 0; j < n; ++j) {
                                sum_col0_0[j] += sum_col0_1[i * n + j];
                        }
                }
                for(int i = 0; i < n; ++i) {
                        error_0 += fabs(sum_col0_0[i] - sum_col0_2[i]);
                }
		//printf("error_0 = %f.\n", error_0);
                //printMV(uot_0, distri_row_0, distri_col_0, sum_col0_0, sum_col0_1, sum_col0_2, m, n);
                count_0++;
        }
        double finish_0 = cpuSecond() - start_0;
        printf("%.3f, ", finish_0 * 1000 / count_0);
        //printMV(uot_0, distri_row_0, distri_col_0, sum_col0_0, sum_col0_1, sum_col0_2, m, n);
        free(thread_handles);
}

static void* UOT_iter(void* arguments) {
        struct arg_struct *args = (struct arg_struct*) arguments;
        double *uot = args->uot;
        double *distri_row = args->distri_row;
        double *distri_col = args->distri_col;
        double *sum_col0 = args->sum_col0;
        double *sum_col1 = args->sum_col1;
        double *sum_col2 = args->sum_col2;
        int m = args->m;
        int n = args->n;
        int rank = args->thread;
        double fi = args->fi;
        int local_m = m / thread_count;
        int start_row = rank * local_m;
        int finish_row = (rank + 1) * local_m;
        double *p_uot = uot + start_row * n;
        double *p_sum_col0 = sum_col0;
        double *p_sum_col1 = sum_col1 + n * rank + n - 1;
        for(int i = start_row; i < finish_row; ++i) {
                double sum_row = 0;
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
                p_uot += n + 1;
                p_sum_col0 -= n;
                p_sum_col1 += n;
        }
        return NULL;
}
