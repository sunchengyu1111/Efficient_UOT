#ifndef _MAPUOT_CPU_H
#define _MAPUOT_CPU_H

#include "MAP-UOT_Testprint_0.h"
#include "MAP-UOT_Testtypical_0.h"
#include "MAP-UOT_parameter.h"
#include "MAP-UOT_computeerror.hpp"
#include "MAP-UOT_TestMap.h"

int thread_count = 0;

double cpuSecond() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return ((double)tp.tv_sec + (double)tp.tv_usec * 1e-6);
}

#endif
