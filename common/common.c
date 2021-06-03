#include "common.h"
#include <stdio.h>
#include <unistd.h>

long double toSeconds(struct timespec start, struct timespec end) {
    long long difSec = end.tv_sec - start.tv_sec;
    long long difNSec = end.tv_nsec - start.tv_nsec;
    long long totalNS = difSec*1000000000L + difNSec;
    return (long double) totalNS / 1000000000.0;
}


double** newPlane(unsigned int n) {
    double** plane = ( double** )malloc(n * sizeof(double*));
    for (unsigned int i = 0; i < n; ++i)
        plane[i] = ( double* )malloc(n * sizeof(double));
    return plane;
}


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


// Parse command-line arguments
struct arguments parseArguments(int argc, char **argv) {
    struct arguments args;
    args.valid = false;
    int opt;

    while ((opt = getopt (argc, argv, "u:d:l:r:s:p:h:x")) != -1) {
        switch (opt) {
            case 'u':
                args.top = atof(optarg);
                break;
            case 'd':
                args.bottom = atof(optarg);
                break;
            case 'l':
                args.left = atof(optarg);
                break;
            case 'r':
                args.right = atof(optarg);
                break;
            case 's':
                args.sizeOfPlane = (unsigned int) atoi(optarg);
                break;
            case 'p':
                args.tolerance = atof(optarg);
                break;
            case 'h':
                // TODO print help stuff
                printf("TODO help info\n");
                break;
            case 'x':
                args.debug = true;
                break;
            case '?':
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return args;
            default:
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return args;
        }
    }

    // Size of the plane must be at least 3x3
    if(args.sizeOfPlane < 3) {
        fprintf (stderr, "The size of the plane must be greater than 2\n");
        return args;
    }
    // Tolerance must be greater than 0, or ends with exit code 1
    if(args.tolerance < 0) {
        fprintf (stderr, "The tolerance must be greater than 0\n");
        return args;
    }
}