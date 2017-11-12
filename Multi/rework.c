#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

typedef struct ThreadData
{
    double** plane;
    unsigned int sizeOfPlane;
    double tolerance;
    unsigned int id;
    unsigned int threadCount;
} ThreadData;

pthread_barrier_t barrierGeneric;
bool finishedFlag;



int main(int argc, char **argv)
{
    unsigned int sizeOfPlane;
    double tolerance;
    double left;
    double right;
    double top;
    double bottom;
    unsigned int threadCount;

    unsigned long iterations;

    ThreadData* threadData;

    // For timing algorithm
    struct timespec start, end;

    double** plane;

    unsigned int i;

    sizeOfPlane = 3;
    tolerance = 0.00001;
    left = 4;
    right = 2;
    top = 1;
    bottom = 3;
    threadCount = 4;

    int opt;
    bool debug = false;

    while ((opt = getopt (argc, argv, "u:d:l:r:s:p:t:h:x")) != -1)
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
            case 't':
                threadCount = (unsigned int) atoi(optarg);
                break;
            case 'h':
                // TODO print help stuff
                printf("TODO help info\n");
                break;
            case 'x':
                debug = true;
                break;
            default:
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

    return 0;
}