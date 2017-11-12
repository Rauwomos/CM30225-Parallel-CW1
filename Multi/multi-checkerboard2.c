#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

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


// ------------------------------------------------------------------------------------------------------------------

long double toSeconds(struct timespec start, struct timespec end) {
    long long difSec = end.tv_sec - start.tv_sec;
    long long difNSec = end.tv_nsec - start.tv_nsec;
    long long totalNS = difSec*1000000000L + difNSec;
    return (long double) totalNS / 1000000000.0;
}

// TODO proper doc string
// Generates a 2D array of uninitialised doubles
double** newPlane(unsigned int n) {
    double** plane = ( double** )malloc(n * sizeof(double*));
    for (unsigned int i = 0; i < n; ++i)
        plane[i] = ( double* )malloc(n * sizeof(double));

    return plane;
}

void freePlane(unsigned int n, double** plane) {
    for (unsigned int i = 0; i < n; ++i)
        free(plane[i]);
    free(plane);
    return;
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

void relaxPlaneRow(double** plane, unsigned int sizeOfPlane, double tolerance, unsigned int row, unsigned int start) {
    unsigned int j;
    double pVal;
    for(j=start; j<sizeOfPlane-1; j+=2) {
        pVal = plane[row][j];
        plane[row][j] = (plane[row-1][j] + plane[row+1][j] + plane[row][j-1] + plane[row][j+1])/4;
        if(finishedFlag && tolerance < fabs(plane[row][j]-pVal)) {
            finishedFlag = false;
        }
    }
}

void relaxPlaneRowRev(double** plane, unsigned int sizeOfPlane, double tolerance, unsigned int row, unsigned int start) {
    unsigned int j;
    double pVal;
    for(j=start; j<sizeOfPlane-1; j+=2) {
        pVal = plane[row][j];
        plane[row][j] = (plane[row-1][j] + plane[row+1][j] + plane[row][j-1] + plane[row][j+1])/4;
        if(!finishedFlag && tolerance < fabs(plane[row][j]-pVal)) {
            finishedFlag = true;
        }
    }
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

// Runs the relaxation technique on the 2d array of doubles that it is passed.
void relaxPlaneThread(ThreadData* threadData)
{  
    unsigned int sizeOfInner = threadData->sizeOfPlane-2;
    unsigned int rowsPerThreadS = sizeOfInner/threadData->threadCount+1;
    unsigned int rowsPerThreadE = sizeOfInner/threadData->threadCount;
    unsigned int remainingRows = sizeOfInner - threadData->threadCount *
    rowsPerThreadE;

    unsigned int startingRow;
    unsigned int endingRow;

    if(threadData->id < remainingRows) {
        startingRow = threadData->id * rowsPerThreadS + 1;
        endingRow = startingRow + rowsPerThreadS;
    } else {
        startingRow = threadData->id * rowsPerThreadE + remainingRows + 1;
        endingRow = startingRow + rowsPerThreadE;
    }
    

    unsigned int row;
    while (1) {
        for(row=startingRow; row<endingRow; row++) {
            relaxPlaneRow(threadData->plane, threadData->sizeOfPlane,
               threadData->tolerance, row, (row%2)+1);
        }
        // Barrier for first half of checkerboard
        pthread_barrier_wait(&barrierGeneric);        
        for(row=startingRow; row<endingRow; row++) {
            relaxPlaneRow(threadData->plane, threadData->sizeOfPlane,
               threadData->tolerance, row, ((row+1)%2)+1);
        }
        // Barrier for second half of checkerboard
        pthread_barrier_wait(&barrierGeneric);
        if(finishedFlag) {
            break;
        }
        pthread_barrier_wait(&barrierGeneric);
        for(row=startingRow; row<endingRow; row++) {
            relaxPlaneRowRev(threadData->plane, threadData->sizeOfPlane,
               threadData->tolerance, row, (row%2)+1);
        }
        // Barrier for first half of checkerboard
        pthread_barrier_wait(&barrierGeneric);        
        for(row=startingRow; row<endingRow; row++) {
            relaxPlaneRowRev(threadData->plane, threadData->sizeOfPlane,
               threadData->tolerance, row, ((row+1)%2)+1);
        }
        pthread_barrier_wait(&barrierGeneric);
        if(!finishedFlag) {
            break;
        }
        pthread_barrier_wait(&barrierGeneric);
    }
}

unsigned long relaxPlaneMain(ThreadData* threadData)
{
    unsigned int sizeOfInner = threadData->sizeOfPlane-2;
    unsigned int rowsPerThreadS = sizeOfInner/threadData->threadCount+1;
    unsigned int rowsPerThreadE = sizeOfInner/threadData->threadCount;
    unsigned int remainingRows = sizeOfInner - threadData->threadCount*rowsPerThreadE;

    unsigned int startingRow;
    unsigned int endingRow;
    unsigned long iterations = 0;

    if(threadData->id < remainingRows) {
        startingRow = threadData->id * rowsPerThreadS + 1;
        endingRow = startingRow + rowsPerThreadS;
    } else {
        startingRow = threadData->id * rowsPerThreadE + remainingRows + 1;
        endingRow = startingRow + rowsPerThreadE;
    }
    
    unsigned int row;
    while (1) {
        iterations++;
        for(row=startingRow; row<endingRow; row++) {
            relaxPlaneRow(threadData->plane, threadData->sizeOfPlane,
               threadData->tolerance, row, (row%2)+1);
        }
        // Barrier for first half of checkerboard
        pthread_barrier_wait(&barrierGeneric);        
        for(row=startingRow; row<endingRow; row++) {
            relaxPlaneRow(threadData->plane, threadData->sizeOfPlane,
               threadData->tolerance, row, ((row+1)%2)+1);
        }
        // Barrier for second half of checkerboard
        pthread_barrier_wait(&barrierGeneric);
        if(finishedFlag) {
            return iterations;
        }
        pthread_barrier_wait(&barrierGeneric);
        
        iterations++;
        for(row=startingRow; row<endingRow; row++) {
            relaxPlaneRowRev(threadData->plane, threadData->sizeOfPlane,
               threadData->tolerance, row, (row%2)+1);
        }
        // Barrier for first half of checkerboard
        pthread_barrier_wait(&barrierGeneric);        
        for(row=startingRow; row<endingRow; row++) {
            relaxPlaneRowRev(threadData->plane, threadData->sizeOfPlane,
               threadData->tolerance, row, ((row+1)%2)+1);
        }
        pthread_barrier_wait(&barrierGeneric);
        if(!finishedFlag) {
            return iterations;
        }
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

    clock_gettime(CLOCK_MONOTONIC, &end);

    // If I wanted the main thread to wait until all of child threads had
    // finished before ending the program. Due to how I've made this program,
    // relaxPlaneMain will not finish until all the child threads
    for(i=0; i<threadCount-1; i++) {
    /* wait for the second thread to finish */
        if(pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }

    pthread_barrier_destroy(&barrierGeneric);

    // clock_gettime(CLOCK_MONOTONIC, &end);

    if(debug)
        printPlane(plane, sizeOfPlane);

    freePlane(sizeOfPlane, plane);
    free(threadData);

    printf("Threads: %d\n", threadCount);
    printf("Size of Pane: %d\n", sizeOfPlane);
    printf("Iterations: %lu\n", iterations);
    printf("Time: %Lfs\n", toSeconds(start, end));
    return 0;
}