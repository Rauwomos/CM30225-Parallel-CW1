#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "immintrin.h"

int main(void)
{   
    double a,b,c,d;
    double i,j,k,l;

    __m256d v1 = _mm256_setr_pd (double d, double c, double b, double a);
    __m256d v2 = _mm256_setr_pd (double d, double c, double b, double a);

    __m256d result = _mm256_add_pd(v1, v2);

    printf("add: %d\n", result);

    return 0;
}