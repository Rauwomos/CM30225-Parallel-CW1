#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "immintrin.h"

bool endFlag = true;


long double toSeconds(struct timespec start, struct timespec end) {
    long long difSec = end.tv_sec - start.tv_sec;
    long long difNSec = end.tv_nsec - start.tv_nsec;
    long long totalNS = difSec*1000000000L + difNSec;
    return (long double) totalNS / 1000000000.0;
}

// TODO propper doc string
// Generates a 2D array of uninitialised doubles
double** newPlane(unsigned int n) {
    double** plane  = ( double** )malloc(n * sizeof(double*));
    plane[0] = ( double * )malloc(n * n * sizeof(double));
 
    for(unsigned int i = 0; i < n; i++)
        plane[i] = (*plane + n * i);

    return plane;
}

// TODO propper doc string
// Populates the plane's wallswith the values provided, and sets the centre parts to zero. If debug is true then it prints outs the array
double** populatePlane(double** plane, unsigned int sizeOfPlane, double top, double bottom, double farLeft, double farRight)
{   
    // Generate 2d array of doubles
    for(unsigned int j=0; j<sizeOfPlane; j++) {
        for(unsigned int i=0; i<sizeOfPlane; i++) {
            if(i == 0) {
                // Left
                plane[i][j] = farLeft;
            } else if(j == 0) {
                // Top
                plane[i][j] = top;
            } else if(i == sizeOfPlane-1) {
                // Right
                plane[i][j] = farRight;
            } else if(j == sizeOfPlane-1) {
                // Bottom
                plane[i][j] = bottom;
            } else {
                plane[i][j] = 0;
            }
        }
    }
    return plane;
}

void printPlane(double** plane, unsigned int sizeOfPlane) {
    for(unsigned int x=0; x<sizeOfPlane; x++) {
        for(unsigned int y=0; y<sizeOfPlane; y++) {
            printf("%f, ", plane[x][y]);
        }
        printf("\n");
    }
    printf("\n");
}

void relaxRow(unsigned int iMax, unsigned int jMax, double** plane, double tolerance, unsigned int offset) {
    unsigned int i,j,k;
    double pVal;
    __m256d v1,v2,r1,r2,four;
    four = _mm256_set_pd(4.0,4.0,4.0,4.0);

    for(i=1; i<iMax; i++){
        for(j=((i+offset)%2)+1; j<jMax; j+=8) {
            // Up Down
            v1 = _mm256_set_pd(plane[i-1][j],plane[i-1][j+2],plane[i-1][j+4],plane[i-1][j+6]);
            v2 = _mm256_set_pd(plane[i+1][j],plane[i+1][j+2],plane[i+1][j+4],plane[i+1][j+6]);
            r1 = _mm256_add_pd(v1,v2);
            // Left Right
            v1 = _mm256_set_pd(plane[i][j-1],plane[i][j+1],plane[i][j+3],plane[i][j+5]);
            v2 = _mm256_set_pd(plane[i][j+1],plane[i][j+3],plane[i][j+5],plane[i][j+7]);
            r2 = _mm256_add_pd(v1,v2);
            // total
            r1 = _mm256_add_pd(r1,r2);
            r1 = _mm256_div_pd(r1,four);
            // tolerance
            if(endFlag) {
                r2 = _mm256_set_pd(plane[i][j],plane[i][j+2],plane[i][j+4],plane[i][j+6]);
                r2 = _mm256_sub_pd(r1, r2);
                for(k=0; k<4; k++) {
                    if(fabs(r2[k])>tolerance) {
                        endFlag = false;
                        break;
                    }
                }
            }
            plane[i][j] = r1[3];
            plane[i][j+2] = r1[2];
            plane[i][j+4] = r1[1];
            plane[i][j+6] = r1[0];
        }
        for(j=((i+offset)%2)+jMax; j<iMax; j+=2) {
            pVal = plane[i][j];
            plane[i][j] = (plane[i-1][j] + plane[i+1][j] + plane[i][j-1] + plane[i][j+1])/4;
            if(endFlag && tolerance < fabs(plane[i][j]-pVal)) {
                endFlag = false;
            }
        }
    }

}

// TODO propper doc string
// Runs the relaxation technique on the 2d array of doubles that it is passed.
unsigned long relaxPlane(double** plane, unsigned int sizeOfPlane, double tolerance)
{
    // bool endFlag;
    unsigned long iterations = 0;
    unsigned int sizeOfInner,remaindingItemsPR,iMax,jMax;
    // double pVal;
    // __m256d v1,v2,r1,r2,four;

    // four = _mm256_set_pd(4.0,4.0,4.0,4.0);
    sizeOfInner = sizeOfPlane-2;
    remaindingItemsPR = sizeOfInner%8;
    iMax = sizeOfPlane-1;
    jMax = iMax-remaindingItemsPR;

    while (1) {
        endFlag = true;
        iterations++;
        relaxRow(iMax,jMax,plane,tolerance,0);
        relaxRow(iMax,jMax,plane,tolerance,1);
        if(endFlag) {
            break;
        }
    }

    return iterations;
}

int main(int argc, char **argv)
{
    unsigned int sizeOfPlane = 10;
    double tolerance = 0.00001;
    double left = 4;
    double right = 2;
    double top = 1;
    double bottom = 3;

    // For timing algorithm
    struct timespec start, end;

    double** plane;

    unsigned long iterations;

    int opt;
    bool debug = false;

    while ((opt = getopt (argc, argv, "u:d:l:r:s:p:h:x")) != -1)
        switch (opt) {
            case 'u':
                top = atof(optarg);
                break;
            case 'd':
                bottom = atof(optarg);
                break;
            case 'l':
                left = atof(optarg);
                break;
            case 'r':
                right = atof(optarg);
                break;
            case 's':
                sizeOfPlane = (unsigned int) atoi(optarg);
                break;
            case 'p':
                tolerance = atof(optarg);
                break;
            case 'h':
                // TODO print help stuff
                printf("TODO help info\n");
                break;
            case 'x':
                debug = true;
                break;
            case '?':
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
    }

    // Size of the plane must be at least 3x3
    if(sizeOfPlane < 3) {
        fprintf (stderr, "The size of the plane must be greater than 2\n");
        return 1;
    }
    // Tolerance must be greater than 0, or ends with exit code 1
    if(tolerance < 0) {
        fprintf (stderr, "The tolerance must be greater than 0\n");
        return 1;
    }

    plane = newPlane(sizeOfPlane);
    plane = populatePlane(plane, sizeOfPlane, left, right, top, bottom);

    clock_gettime(CLOCK_MONOTONIC, &start);
    iterations = relaxPlane(plane, sizeOfPlane, tolerance);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if(debug)
        printPlane(plane, sizeOfPlane);

    free(*plane);
    free(plane);

    printf("Threads: 1\n");
    printf("Size of Pane: %d\n", sizeOfPlane);
    printf("Iterations: %lu\n", iterations);
    printf("Time: %Lfs\n", toSeconds(start, end));
    return 0;
}