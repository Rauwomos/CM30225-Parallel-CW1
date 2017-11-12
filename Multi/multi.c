#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


typedef struct Results
{
    double time_spent;
    unsigned long iterations;
} Result;

typedef struct ThreadDatas
{
    double** plane;
    unsigned int sizeOfPlane;
    double tolerance;
    unsigned int id;
    unsigned int threadCount;
} ThreadData;

pthread_barrier_t barrierGeneric;
// pthread_barrier_t barrierCalc;
// pthread_barrier_t barrierTolerance;
// pthread_barrier_t barrierFlag;
bool finishedFlag = true;


// ------------------------------------------------------------------------------------------------------------------

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

void printPlane(double** plane, unsigned int sizeOfPlane) {
    for(unsigned int x=0; x<sizeOfPlane; x++) {
        for(unsigned int y=0; y<sizeOfPlane; y++) {
            printf("%f, ", plane[x][y]);
        }
        printf("\n");
    }
    printf("\n");
}

// TODO propper doc string
// Runs the relaxation technique on the 2d array of doubles that it is passed.
void relaxPlaneThread(ThreadData* threadData)
{  

    unsigned int sizeOfInner = threadData->sizeOfPlane-2;
    unsigned int rowsPerThread = sizeOfInner/threadData->threadCount;
    unsigned int remainingRows = sizeOfInner - threadData->threadCount * rowsPerThread;
    unsigned int remainingItems = remainingRows * sizeOfInner;
    unsigned int remainingItemsMost = remainingItems/threadData->threadCount;

    unsigned int startingRow = threadData->id * rowsPerThread+1;
    unsigned int endingRow = startingRow + rowsPerThread;
    
    unsigned int startingRemainder = threadData->id * remainingItemsMost;
    unsigned int endingRemainder = startingRemainder + remainingItemsMost;

    unsigned int i,j;

    double pVal;
    while (1) {
        // This accesses this threads rows for the contiguous blocks of memory bits
        for(j=1; j<threadData->sizeOfPlane-1; j++) {
            for(i=startingRow; i<endingRow; i++) {
            // for(i=1; i<threadData->sizeOfPlane-1; i++) {
                pVal = threadData->plane[i][j];
                threadData->plane[i][j] = (threadData->plane[i-1][j] + threadData->plane[i+1][j] + threadData->plane[i][j-1] + threadData->plane[i][j+1])/4;
                if(finishedFlag && threadData->tolerance < fabs(threadData->plane[i][j]-pVal)) {
                    finishedFlag = false;
                }
            }
        }
        for(unsigned int n = startingRemainder; n<endingRemainder; n++) {
            i = n%sizeOfInner + 1;
            j = n/sizeOfInner + sizeOfInner - remainingRows + 1;
            pVal = threadData->plane[i][j];
            threadData->plane[i][j] = (threadData->plane[i-1][j] + threadData->plane[i+1][j] + threadData->plane[i][j-1] + threadData->plane[i][j+1])/4;
            if(finishedFlag && threadData->tolerance < fabs(threadData->plane[i][j]-pVal)) {
                finishedFlag = false;
            }
        }

        pthread_barrier_wait(&barrierGeneric);
        if(finishedFlag) {
            break;
        }
        pthread_barrier_wait(&barrierGeneric);
        pthread_barrier_wait(&barrierGeneric);
    }
}

unsigned long relaxPlaneMain(ThreadData* threadData)
{  
    unsigned int sizeOfInner = threadData->sizeOfPlane-2;
    unsigned int rowsPerThread = sizeOfInner/threadData->threadCount;
    unsigned int remainingRows = sizeOfInner - threadData->threadCount * rowsPerThread;
    unsigned int remainingItems = remainingRows * sizeOfInner;
    unsigned int remainingItemsMost = remainingItems/threadData->threadCount;
    unsigned int remainingItemsOutlier = remainingItems - (remainingItemsMost * (threadData->threadCount-1));

    unsigned int startingRow = threadData->id * rowsPerThread+1;
    unsigned int endingRow = startingRow + rowsPerThread;
    
    unsigned int startingRemainder = threadData->id * remainingItemsMost;
    // Don't need to minus one as I am testing for less than remainder in for loop
    unsigned int endingRemainder = startingRemainder + remainingItemsOutlier;

    unsigned long iterations = 0;
    unsigned int i,j;
    
    double pVal;
    while (1) {
        iterations++;
        // This accesses this threads rows for the contiguous blocks of memory bits
        for(j=1; j<threadData->sizeOfPlane-1; j++) {
            for(i=startingRow; i<endingRow; i++) {
            // for(i=1; i<threadData->sizeOfPlane-1; i++) {
                pVal = threadData->plane[i][j];
                threadData->plane[i][j] = (threadData->plane[i-1][j] + threadData->plane[i+1][j] + threadData->plane[i][j-1] + threadData->plane[i][j+1])/4;
                if(finishedFlag && threadData->tolerance < fabs(threadData->plane[i][j]-pVal)) {
                    finishedFlag = false;
                }
            }
        }
        for(unsigned int n = startingRemainder; n<endingRemainder; n++) {
            i = n%sizeOfInner + 1;
            j = n/sizeOfInner + sizeOfInner - remainingRows + 1;
            pVal = threadData->plane[i][j];
            threadData->plane[i][j] = (threadData->plane[i-1][j] + threadData->plane[i+1][j] + threadData->plane[i][j-1] + threadData->plane[i][j+1])/4;
            if(finishedFlag && threadData->tolerance < fabs(threadData->plane[i][j]-pVal)) {
                finishedFlag = false;
            }
        }

        pthread_barrier_wait(&barrierGeneric);
        if(finishedFlag) {
            printPlane(threadData->plane, threadData->sizeOfPlane);
            return iterations;
        }
        pthread_barrier_wait(&barrierGeneric);
        finishedFlag = true;
        pthread_barrier_wait(&barrierGeneric);
    }
    return iterations;
}

int main(void)
{
    unsigned int sizeOfPlane = 500;
    double tolerance = 0.00001;
    unsigned int threadCount = 4;

    double left = 2.0;
    double right = 4.0;
    double top = 1.0;
    double bottom = 3.0;

    // For timing algorithm
    struct timespec start, end;

    double** plane;

    pthread_t threads[threadCount];
    ThreadData* threadData;

    unsigned int i;

    unsigned long iterations;


    // Size of the plane must be at least 3x3
    if(sizeOfPlane < 3)
        sizeOfPlane = 3;
    // Tolerance must be greater than 0, or ends with exit code 1
    if(tolerance < 0)
        return 1;

    plane = newPlane(sizeOfPlane);
    plane = populatePlane(plane, sizeOfPlane, left, right, top, bottom);

    // Thread stuff bellow here
    // ----------------------------------------------------------

    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_barrier_init(&barrierGeneric, NULL, threadCount);

    threadData = malloc(threadCount * sizeof(ThreadData));
    for (i = 0; i < threadCount; ++i) {
        threadData[i].sizeOfPlane = sizeOfPlane;
        threadData[i].id = i;
        threadData[i].threadCount = threadCount;
        threadData[i].plane = plane;
        threadData[i].tolerance = tolerance;
    }

    for(i=0; i<threadCount-1; i++) {
      /* create a second thread which executes inc_x(&x) */
        if(pthread_create(&threads[i], NULL, (void*(*)(void*))relaxPlaneThread, &threadData[i])) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    iterations = relaxPlaneMain(&threadData[threadCount-1]);

    for(i=0; i<threadCount-1; i++) {
    /* wait for the second thread to finish */
        if(pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }

    pthread_barrier_destroy(&barrierGeneric);

    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Iterations: %lu\n", iterations);
    printf("Time: %Lfs\n", toSeconds(start, end));

    return 0;
}

// TODO make it split rows differently