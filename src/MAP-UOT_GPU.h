#ifndef _MAPUOT_GPU_H
#define _MAPUOT_GPU_H

#include "MAP-UOT_Testprint_1.h"
#include "MAP-UOT_Testtypical_1.h"
#include "MAP-UOT_parameter.h"
#include "MAP-UOT_computeerror.hpp"
#include "MAP-UOT_TestGPUMap.h"

#define CHECK(call){                                                                    \
        const cudaError_t error = call;                                                 \
        if(error != cudaSuccess) {                                                      \
                printf("Error: %s.%d.\n", __FILE__, __LINE__);                          \
                printf("Code: %d. Reason: %s.\n", error, cudaGetErrorString(error));    \
                exit(-10 * error);                                                      \
        }                                                                               \
}

double cpuSecond() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return ((double)tp.tv_sec + (double)tp.tv_usec * 1e-6);
}

#endif
