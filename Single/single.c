#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

long double toSeconds(struct timespec start, struct timespec end) {
    long long difSec = end.tv_sec - start.tv_sec;
    long long difNSec = end.tv_nsec - start.tv_nsec;
    long long totalNS = difSec*1000000000L + difNSec;
    return (long double) totalNS / 1000000000.0;
}

// TODO propper doc string
// Generates a 2D array of uninitialised doubles
double** newPlane(unsigned int n) {
    double** plane = ( double** )malloc(n * sizeof(double*));
    for (unsigned int i = 0; i < n; ++i)
        plane[i] = ( double* )malloc(n * sizeof(double));
    return plane;
}

// TODO propper doc string
// Populates the plane's wallswith the values provided, and sets the centre parts to zero. If debug is true then it prints outs the array
double** populatePlane(double** plane, unsigned int sizeOfPlane, double farLeft, double farRight, double top, double bottom)
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

// TODO propper doc string
// Runs the relaxation technique on the 2d array of doubles that it is passed.
unsigned long relaxPlane(double** plane, unsigned int sizeOfPlane, double tolerance)
{
    unsigned long iterations = 0;
    unsigned int i,j;
    double pVal;
    bool endFlag;
    do {
        endFlag = true;
        iterations++;
        // i then j, accessing continues blocks of memory. Increases speed by 1/3
        for(i=1; i<sizeOfPlane-1; i++) {
            for(j=1; j<sizeOfPlane-1; j++) {
                pVal = plane[i][j];
                plane[i][j] = (plane[i-1][j] + plane[i+1][j] + plane[i][j-1] + plane[i][j+1])/4;
                if(endFlag && tolerance < fabs(plane[i][j]-pVal)) {
                    endFlag = false;
                }
            }
        }
    } while(!endFlag);
    
    return iterations;
}

int main(int argc, char **argv)
{
    unsigned int sizeOfPlane;
    double tolerance;
    double left;
    double right;
    double top;
    double bottom;

    unsigned long iterations;

    // For timing algorithm
    struct timespec start, end;

    double** plane;

    sizeOfPlane = 100;
    tolerance = 0.000001;
    left = 4;
    right = 2;
    top = 1;
    bottom = 3;

    int opt;
    bool argsSet[7] = {true,true,true,true,true,true,true};
    bool debug = false;

    while ((opt = getopt (argc, argv, "u:d:l:r:s:p:h:x")) != -1)
        switch (opt) {
            case 'u':
                top = atof(optarg);
                argsSet[0] = false;
                break;
            case 'd':
                bottom = atof(optarg);
                argsSet[1] = false;
                break;
            case 'l':
                left = atof(optarg);
                argsSet[2] = false;
                break;
            case 'r':
                right = atof(optarg);
                argsSet[3] = false;
                break;
            case 's':
                sizeOfPlane = (unsigned int) atoi(optarg);
                argsSet[4] = false;
                break;
            case 'p':
                tolerance = atof(optarg);
                argsSet[5] = false;
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

    // for(int i=0; i<6; i++) {
    //     if(argsSet[i]) {
    //         fprintf (stderr, "All arguments must be set\n");
    //         // TODO print help stuff
    //         printf("TODO help info\n");
    //         return 1;
    //     }
    // }

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

    // Initialise the 2d array of doubles
    plane = newPlane(sizeOfPlane);
    plane = populatePlane(plane, sizeOfPlane, left, right, top, bottom);
    clock_gettime(CLOCK_MONOTONIC, &start);
    iterations = relaxPlane(plane, sizeOfPlane, tolerance);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if(debug) {
        printf("plane");
    }

    printf("Threads: 1\n");
    printf("Size of Plane: %d\n", sizeOfPlane);
    printf("Iterations: %lu\n", iterations);
    printf("Time: %Lfs\n", toSeconds(start, end));
    return 0;
}