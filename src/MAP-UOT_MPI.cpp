#include "MAP-UOT_MPI.h"

int main(int argc, char *argv[]) {
	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	int world_rank, world_size;
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int w = world_size;

	// data preprocess -- MAP-UOT
	m = m / w;
	double *uot_0, *distri_row_0, *distri_col_0, *sum_col0_0, *sum_col0_1, *sum_col0_2, *t;
	uot_0 = new double[m * n];
        distri_row_0 = new double[m];
        distri_col_0 = new double[n];
        sum_col0_0 = new double[n];
        sum_col0_1 = new double[n];
        sum_col0_2 = new double[n];
        t = new double[m * w];
	srand(time(NULL) + world_rank);
	for(int i = 0; i < m * n; ++i) {
		uot_0[i] = (double)rand() / (double)(RAND_MAX) * 100;                      
	}
	if(world_rank == 0) {
		for(int i = 0; i < m * w; ++i) {
			t[i] = (double)rand() / (double)(RAND_MAX) * 100;
		}
		for(int i = 0; i < n; ++i) {
			distri_col_0[i] = (double)rand() / (double)(RAND_MAX) * 100;
		}
	}
	MPI_Scatter(t, m, MPI_DOUBLE, distri_row_0, m, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(distri_col_0, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	delete []t;
	int count_0 = 0;
	double error_0 = 1;
	double fi = 0.5;
	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < m; ++j) {
			sum_col0_1[i] += uot_0[i + j * n];
		}	
	}
	MPI_Allreduce(sum_col0_1, sum_col0_0, n, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	//printMV(uot_0, distri_row_0, distri_col_0, sum_col0_0, sum_col0_1, m, n);
		
	// data preprocess -- Typical-UOT
	double *uot_1 = new double[m * n];
        double *distri_row_1 = new double[m * w];
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
	UOT_MPIMAP(uot_0, distri_row_0, distri_col_0, sum_col0_0, sum_col0_1, sum_col0_2, m, n, w, fi, error_0, count_0, world_rank);
		
	// compute error
	//MPI_Barrier(MPI_COMM_WORLD);
	//if(world_rank == 0) compute_error(uot_0, uot_1, m, n);
	//MPI_Barrier(MPI_COMM_WORLD);
	
	// end
	MPI_Finalize();
	delete []uot_0;
	delete []distri_row_0;
	delete []distri_col_0;
	delete []sum_col0_0;
	delete []sum_col0_1;
	delete []sum_col0_2;
}
