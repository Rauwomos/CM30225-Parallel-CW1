#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "../common/common.h"

// #define DEBUG

// TODO propper doc string
// Runs the relaxation technique on the 2d array of doubles that it is passed.
unsigned long relaxPlane(double** plane, unsigned int sizeOfPlane, double tolerance)
{
    unsigned long iterations = 0;
    unsigned int i,j;
    double pVal;
    bool endFlag;
    while (1) {
        endFlag = true;
        iterations++;
        // i then j, accessing continues blocks of memory. Increases speed by 1/3
        for(i=1; i<sizeOfPlane-1; i++) {
            for(j=(i%2)+1; j<sizeOfPlane-1; j+=2) {
                pVal = plane[i][j];
                plane[i][j] = (plane[i-1][j] + plane[i+1][j] + plane[i][j-1] + plane[i][j+1])/4;
                if(endFlag && tolerance < fabs(plane[i][j]-pVal)) {
                    endFlag = false;
                }
            }
        }
        for(i=1; i<sizeOfPlane-1; i++) {
            for(j=((i+1)%2)+1; j<sizeOfPlane-1; j+=2) {
                pVal = plane[i][j];
                plane[i][j] = (plane[i-1][j] + plane[i+1][j] + plane[i][j-1] + plane[i][j+1])/4;
                if(endFlag && tolerance < fabs(plane[i][j]-pVal)) {
                    endFlag = false;
                }
            }
        }
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

    struct timespec start, end;
    double** plane;
    unsigned long iterations;

    plane = newPlane(sizeOfPlane);
    plane = populatePlane(plane, sizeOfPlane, left, right, top, bottom);

    clock_gettime(CLOCK_MONOTONIC, &start);
    iterations = relaxPlane(plane, sizeOfPlane, tolerance);
    clock_gettime(CLOCK_MONOTONIC, &end);

    #ifdef DEBUG
    printPlane(plane, sizeOfPlane);
    #endif /* DEBUG */

    printf("Threads: 1\n");
    printf("Size of Pane: %d\n", sizeOfPlane);
    printf("Iterations: %lu\n", iterations);
    printf("Time: %Lfs\n", toSeconds(start, end));
    return 0;
}