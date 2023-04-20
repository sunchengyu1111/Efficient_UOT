#ifndef _MAPUOT_COMPUTEERROR_H
#define _MAPUOT_COMPUTEERROR_H

#include <stdio.h>
#include <math.h>

template <typename T>
void compute_error(T *uot_0, T *uot_1, int m, int n) {
        T error = 0;
        for(int i = 0; i < m * n; ++i) {
                error += abs(uot_0[i] - uot_1[i]);
        }
        printf("error = %f.\n", error);
}

#endif
