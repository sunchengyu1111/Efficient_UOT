#include "MAP-UOT_TestGPUMap.h"

void UOT_GPUMAP(float *uot_0, float *distri_row_0, float *distri_col_0, float *sum_row0_0, float *sum_col0_0, float *sum_col0_1, float error_0, const int m, const int n, const double fi, int count_0) {
	float *d_uot_0, *d_distri_row_0, *d_distri_col_0, *d_sum_row0_0, *d_sum_col0_0, *d_sum_col0_1, *d_error_0;
        cudaMalloc((float**)&d_uot_0, m * n * sizeof(float));
        cudaMalloc((float**)&d_distri_row_0, m * sizeof(float));
        cudaMalloc((float**)&d_distri_col_0, n * sizeof(float));
        cudaMalloc((float**)&d_sum_row0_0, m * sizeof(float));
        cudaMalloc((float**)&d_sum_col0_0, n * sizeof(float));
        cudaMalloc((float**)&d_sum_col0_1, n * sizeof(float));
        cudaMalloc((float**)&d_error_0, sizeof(float));
        cudaMemcpy(d_uot_0, uot_0, m * n * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_distri_row_0, distri_row_0, m * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_distri_col_0, distri_col_0, n * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_sum_row0_0, sum_row0_0, m * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_sum_col0_0, sum_col0_0, n * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_sum_col0_1, sum_col0_1, n * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_error_0, &error_0, sizeof(float), cudaMemcpyHostToDevice);
        // parameter
	double start = 0, finish = 0; // ,start0, finish0, start1, finish1;
        const int temp = 128;
        const int c_block_x = 128;
        const int c_iter_y = 8;
        dim3 c_block(c_block_x);
        // add '/4'
        dim3 c_grid((n + c_block_x - 1) / c_block_x, (m + c_iter_y - 1) / c_iter_y);
        const int r_block_x = 128;
        const int r_block_y = 2;
        const int r_iter_y = 8;
        dim3 r_block(r_block_x, r_block_y);
        // add '/4'
        dim3 r_grid((n + r_block_x - 1) / r_block_x, (m + r_block_y * r_iter_y - 1) / (r_block_y * r_iter_y));
        double time = 0; // ,time0 = 0, time1 = 0;
	while(error_0 >= eps) {
                //printf("count_0 = %d.\n", count_0);
                count_0++;
                start = cpuSecond();
                cudaMemset(d_error_0, 0, sizeof(float));
                cudaMemcpy(d_sum_col0_1, d_sum_col0_0, n * sizeof(float), cudaMemcpyDeviceToDevice);
                modify_col_factor<<<(n + temp - 1) / temp, temp>>>(d_distri_col_0, d_sum_col0_0, fi);
                cudaMemset(d_sum_row0_0, 0, m * sizeof(float));
                UOT_iterbase_colsum<<<c_grid, c_block>>>(d_uot_0, d_sum_row0_0, d_sum_col0_0, m, n, c_iter_y);
                modify_row_factor<<<(n + temp - 1) / temp, temp>>>(d_distri_row_0, d_sum_row0_0, fi);
                cudaMemset(d_sum_col0_0, 0, n * sizeof(float));
                UOT_iterbase_rowsum<<<r_grid, r_block>>>(d_uot_0, d_sum_row0_0, d_sum_col0_0, m, n, r_iter_y);
                UOT_error<<<(n + temp - 1) / temp, temp>>>(d_sum_col0_0, d_sum_col0_1, d_error_0);
                cudaDeviceSynchronize();
                finish = cpuSecond() - start;
                time += finish;
                cudaMemcpy(sum_row0_0, d_sum_row0_0, m * sizeof(float), cudaMemcpyDeviceToHost);
                cudaMemcpy(sum_col0_0, d_sum_col0_0, n * sizeof(float), cudaMemcpyDeviceToHost);
                cudaMemcpy(sum_col0_1, d_sum_col0_1, n * sizeof(float), cudaMemcpyDeviceToHost);
                cudaMemcpy(uot_0, d_uot_0, m * n * sizeof(float), cudaMemcpyDeviceToHost);
                cudaMemcpy(&error_0, d_error_0, sizeof(float), cudaMemcpyDeviceToHost);
                //printf("error_0 = %f.\n", error_0);               
                //printMV(uot_0, distri_row_0, distri_col_0, sum_row0_0, sum_col0_0, sum_col0_1, error_0, m, n);
        }
        printf("%f, ", time / count_0);
}

__global__ void modify_row_factor(float *distri_row, float *sum_row, float fi) {
        int id = blockIdx.x * blockDim.x + threadIdx.x;
        sum_row[id] = powf(distri_row[id] / sum_row[id], fi);
}

__global__ void modify_col_factor(float *distri_col, float *sum_col, float fi) {
        int id = blockIdx.x * blockDim.x + threadIdx.x;
        sum_col[id] = powf(distri_col[id] / sum_col[id], fi);
}

__global__ void UOT_error(float *sum_col0, float *sum_col1, float *error) {
        int id = blockIdx.x * blockDim.x + threadIdx.x;
        int laneID = threadIdx.x % 32;
        float t = abs(sum_col1[id] - sum_col0[id]);
        for(unsigned w = 16; w >= 1; w >>= 1) {
                t += __shfl_down_sync(0xffffffff, t, w);
        }
        if(laneID == 0) atomicAdd(error, t);
}

__global__ void UOT_iterbase_rowsum(float *uot, float *sum_row, float *sum_col, int m, int n, int iter_y) {
        // -----change along with blockDim.y----- //
        __shared__ float sm_col[2][128];
        __shared__ float sm_row[16];
        int col = blockDim.x * blockIdx.x + threadIdx.x;
        int row = iter_y * (blockDim.y * blockIdx.y + threadIdx.y);
        int idx = row * n + col;
        sm_col[threadIdx.y][threadIdx.x] = 0;
        // -----change along with iter_y, blockDim.y----- //
        if(threadIdx.x < 16 && threadIdx.y == 0) sm_row[threadIdx.x] = sum_row[threadIdx.x + row];
        __syncthreads();
        int sm_rowid = iter_y * threadIdx.y;
        #pragma unroll
        for(int i = 0; i < iter_y; ++i) {
                float t = uot[idx];
                t *= sm_row[sm_rowid];
                sm_col[threadIdx.y][threadIdx.x] += t;
                uot[idx] = t;
                sm_rowid++;
                idx += n;
        }
        __syncthreads();
        // change along with blockDim.y
        if(threadIdx.y == 0) atomicAdd(&sum_col[col], sm_col[0][threadIdx.x] + sm_col[1][threadIdx.x]); // + sm_col[2][laneID] + sm_col[3][laneID]);
}

__global__ void UOT_iterbase_colsum(float *uot, float *sum_row, float *sum_col, int m, int n, int iter_y) {
        int col = blockDim.x * blockIdx.x + threadIdx.x;
        int row = iter_y * blockIdx.y;
        int idx = row * n + col;
        int laneID = threadIdx.x % 32;
        int offset = threadIdx.x / 32;
        // -----change along with blockDim.x----- //
        __shared__ float sm_col0[128];
        __shared__ float sm_col1[4];
        //float *p_sum_col = &sum_col[col];
        sm_col0[threadIdx.x] = sum_col[col];
        #pragma unroll
        for(int i = 0; i < iter_y; ++i) {
                float t = uot[idx];
                t *= sm_col0[threadIdx.x];
                uot[idx] = t;
                for(unsigned w = 16; w >= 1; w >>= 1) {
                        t += __shfl_down_sync(0xffffffff, t, w);
                }
                if(laneID == 0) sm_col1[offset] = t;
                __syncthreads();
                // -----change along with blockDim.x----- //
                if(threadIdx.x == 0) atomicAdd(&sum_row[row], sm_col1[0] + sm_col1[1] + sm_col1[2] + sm_col1[3]);
                //if(threadIdx.x == 1) atomicAdd(&sum_row[row], sm_col1[4] + sm_col1[5] + sm_col1[6] + sm_col1[7]);
                //if(threadIdx.x == 2) atomicAdd(&sum_row[row], sm_col1[8] + sm_col1[9] + sm_col1[10] + sm_col1[11]);
                //if(threadIdx.x == 3) atomicAdd(&sum_row[row], sm_col1[12] + sm_col1[13] + sm_col1[14] + sm_col1[15]);
                __syncthreads();
                row++;
                idx += n;
        }
}
