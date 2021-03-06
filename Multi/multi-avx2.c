#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "immintrin.h"

typedef struct ThreadDatas
{
    double** plane;
    unsigned int sizeOfPlane;
    double tolerance;
    unsigned int id;
    unsigned int threadCount;
} ThreadData;

pthread_barrier_t barrierGeneric;
bool finishedFlag;
// bool childFinishedFlag;


// ------------------------------------------------------------------------------------------------------------------

long double toSeconds(struct timespec start, struct timespec end) {
    long long difSec = end.tv_sec - start.tv_sec;
    long long difNSec = end.tv_nsec - start.tv_nsec;
    long long totalNS = difSec*1000000000L + difNSec;
    return (long double) totalNS / 1000000000.0;
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

// TODO proper doc string
// Generates a 2D array of uninitialised doubles
double** newPlane(unsigned int n) {
    double** plane  = ( double** )malloc(sizeof(double*) * n);
    plane[0] = (double *)malloc(sizeof(double) * n * n);
 
    for(unsigned int i = 0; i < n; i++)
        plane[i] = (*plane + n * i);

    return plane;
}

// TODO proper doc string
// Populates the plane's walls with the values provided, and sets the centre
// parts to zero. If debug is true then it prints outs the array
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

void relaxRows(unsigned int iMax, unsigned int jMax, double** plane, double tolerance, unsigned int offset, unsigned int startingRow, unsigned int endingRow) {
    unsigned int i,j,k;
    double pVal;
    __m256d v1,v2,r1,r2,four;
    four = _mm256_set_pd(4.0,4.0,4.0,4.0);

    for(i=startingRow; i<endingRow; i++){
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
            if(finishedFlag) {
                r2 = _mm256_set_pd(plane[i][j],plane[i][j+2],plane[i][j+4],plane[i][j+6]);
                r2 = _mm256_sub_pd(r1, r2);
                for(k=0; k<4; k++) {
                    if(fabs(r2[k])>tolerance) {
                        finishedFlag = false;
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
            if(finishedFlag && tolerance < fabs(plane[i][j]-pVal)) {
                finishedFlag = false;
            }
        }
    }
}

// Runs the relaxation technique on the 2d array of doubles that it is passed.
void relaxPlaneThread(ThreadData* threadData)
{  
    printf("Thread ID %d\n", threadData->id);
    unsigned int iMax,jMax,startingRow,endingRow,remainingRows,rowsPerThreadE,rowsPerThreadS,sizeOfInner,remaindingItemsPR;

    sizeOfInner = threadData->sizeOfPlane-2;
    rowsPerThreadS = sizeOfInner/threadData->threadCount+1;
    rowsPerThreadE = sizeOfInner/threadData->threadCount;
    remainingRows = sizeOfInner - threadData->threadCount*rowsPerThreadE;
    remaindingItemsPR = sizeOfInner%8;
    iMax = threadData->sizeOfPlane-1;
    jMax = iMax-remaindingItemsPR;


    if(threadData->id < remainingRows) {
        startingRow = threadData->id * rowsPerThreadS + 1;
        endingRow = startingRow + rowsPerThreadS;
    } else {
        startingRow = threadData->id * rowsPerThreadE + remainingRows + 1;
        endingRow = startingRow + rowsPerThreadE;
    }
    
    while (1) {
        relaxRows(iMax,jMax,threadData->plane,threadData->tolerance,0,startingRow,endingRow);
        // Barrier for first half of checkerboard
        pthread_barrier_wait(&barrierGeneric);        
        relaxRows(iMax,jMax,threadData->plane,threadData->tolerance,1,startingRow,endingRow);
        // Barrier for second half of checkerboard
        pthread_barrier_wait(&barrierGeneric);
        pthread_barrier_wait(&barrierGeneric);
        // Logic for child threads to finish if needed
        // if(childFinishedFlag) {
        //     return;
        // }
    }
}

unsigned long relaxPlaneMain(ThreadData* threadData)
{
    printf("Thread ID %d\n", threadData->id);
    unsigned int iMax,jMax,startingRow,endingRow,remainingRows,rowsPerThreadE,rowsPerThreadS,sizeOfInner,remaindingItemsPR;
    unsigned long iterations = 0;

    sizeOfInner = threadData->sizeOfPlane-2;
    rowsPerThreadS = sizeOfInner/threadData->threadCount+1;
    rowsPerThreadE = sizeOfInner/threadData->threadCount;
    remainingRows = sizeOfInner - threadData->threadCount*rowsPerThreadE;
    remaindingItemsPR = sizeOfInner%8;
    iMax = threadData->sizeOfPlane-1;
    jMax = iMax-remaindingItemsPR;

    if(threadData->id < remainingRows) {
        startingRow = threadData->id * rowsPerThreadS + 1;
        endingRow = startingRow + rowsPerThreadS;
    } else {
        startingRow = threadData->id * rowsPerThreadE + remainingRows + 1;
        endingRow = startingRow + rowsPerThreadE;
    }

    if(threadData->id < remainingRows) {
        startingRow = threadData->id * rowsPerThreadS + 1;
        endingRow = startingRow + rowsPerThreadS;
    } else {
        startingRow = threadData->id * rowsPerThreadE + remainingRows + 1;
        endingRow = startingRow + rowsPerThreadE;
    }
    
    while (1) {
        iterations++;
        relaxRows(iMax,jMax,threadData->plane,threadData->tolerance,0,startingRow,endingRow);
        // Barrier for first half of checkerboard
        pthread_barrier_wait(&barrierGeneric);        
        relaxRows(iMax,jMax,threadData->plane,threadData->tolerance,1,startingRow,endingRow);
        // Barrier for second half of checkerboard
        pthread_barrier_wait(&barrierGeneric);
        if(finishedFlag) {
            // Logic to allow child threads to break if needed
            // childFinishedFlag = true;
            // pthread_barrier_wait(&barrierGeneric);
            return iterations;
        }
        finishedFlag = true;
        pthread_barrier_wait(&barrierGeneric);
    }
}

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

    // Has to be set after threadCount has been initialised
    pthread_t threads[threadCount];


    finishedFlag = true;
    // childFinishedFlag = false;

    // Initialise the 2d array of doubles
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
        if(pthread_create(&threads[i], NULL, (void*(*)(void*))relaxPlaneThread,
           &threadData[i])) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    iterations = relaxPlaneMain(&threadData[threadCount-1]);

    // Stop the timer before joining/freeing threads as this is something
    // that I could just leave to the OS due to how the program runs
    clock_gettime(CLOCK_MONOTONIC, &end);

    // for(i=0; i<threadCount-1; i++) {
    // /* wait for the second thread to finish */
    //     if(pthread_join(threads[i], NULL)) {
    //         fprintf(stderr, "Error joining thread\n");
    //         return 2;
    //     }
    // }

    // pthread_barrier_destroy(&barrierGeneric);

    if(debug)
        printPlane(plane, sizeOfPlane);

    // Logic to free mallocs if needed. As program is about to end, I just leave it to be cleaned up by the OS
    free(*plane);
    free(plane);
    // free(threadData);

    printf("Threads: %d\n", threadCount);
    printf("Size of Pane: %d\n", sizeOfPlane);
    printf("Iterations: %lu\n", iterations);
    printf("Time: %Lfs\n", toSeconds(start, end));
    return 0;
}
