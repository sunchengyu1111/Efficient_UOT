#ifndef _MAPUOT_MPI_H
#define _MAPUOT_MPI_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <immintrin.h>
#include <vector>
#include <cassert>
#include "MAP-UOT_Testprint_0.h"
#include "MAP-UOT_TestMPItypical.h"
#include "MAP-UOT_parameter.h"
#include "MAP-UOT_computeerror.hpp"
#include "MAP-UOT_TestMPIMap.h"

double cpuSecond() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return ((double)tp.tv_sec + (double)tp.tv_usec * 1e-6);
}

#endif
