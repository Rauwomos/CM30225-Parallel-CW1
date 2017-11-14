#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "immintrin.h"

int main(void)
{   

    // double a0 = 1;
    // double a1 = 2;
    // double a2 = 4;
    // double a3 = 8;
    // double b0 = 1;
    // double b1 = 2;
    // double b2 = 4;
    // double b3 = 8;

    double* t1 = malloc(4*sizeof(double*));
    double* t2 = malloc(4*sizeof(double*));

    for(unsigned int i=0; i<4; i++) {
        t1[i] = i;
        t2[i] = i*2;
    }

    // __m256d v1 = _mm256_setr_pd(a0, a1, a2, a3);
    // __m256d v2 = _mm256_setr_pd(b0, b1, b2, b3);

    __m256d v1 = _mm256_loadu_pd((double const *) t1);
    __m256d v2 = _mm256_loadu_pd((double const *) t2);

    __m256d vresult = _mm256_add_pd(v1, v2);

    double* result = (double*)&vresult;

    result[0] = 69;

    double* result2 = (double*)&vresult;    

    printf("c0: %f\n", result2[0]);
    printf("c1: %f\n", result2[1]);
    printf("c2: %f\n", result2[2]);
    printf("c3: %f\n", result2[3]);

    printf("c0: %f\n", vresult[0]);
    printf("c1: %f\n", vresult[1]);
    printf("c2: %f\n", vresult[2]);
    printf("c3: %f\n", vresult[3]);

    return 0;
}