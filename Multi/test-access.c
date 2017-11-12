#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(void)
{   
    unsigned int sizeOfPlane = 13;
    unsigned int threadCount = 3;
    unsigned int threadID = 1;

    unsigned int sizeOfInner = sizeOfPlane-2;
    unsigned int rowsPerThread = sizeOfInner/threadCount;
    unsigned int remainingRows = sizeOfInner - threadCount*rowsPerThread;
    unsigned int remainingItems = remainingRows * sizeOfInner;
    unsigned int remainingItemsMost = remainingItems/threadCount;
    unsigned int remainingItemsOutlier = remainingItems-(remainingItemsMost*(threadCount-1));

    unsigned int startingRemainder = threadID * remainingItemsMost;
    // Don't need to minus one as I am testing for less than remainder in for loop
    unsigned int endingRemainder = startingRemainder + remainingItemsMost;

    unsigned int startingRow = threadID * rowsPerThread+1;
    unsigned int endingRow = startingRow + rowsPerThread;

    printf("Size of Inner: %d\n", sizeOfInner);
    printf("Rows per thread: %d\n", rowsPerThread);
    printf("Remainding Rows: %d\n", remainingRows);
    printf("Remainding Items: %d\n", remainingItems);
    printf("Remainding Items for Most: %d\n", remainingItemsMost);
    printf("Remainding Items for outlier thread: %d\n", remainingItemsOutlier);
    printf("Starting Item of Remainder: %d\n", startingRemainder);
    printf("Ending Item of Remainder: %d\n\n", endingRemainder);
    // for(unsigned int n = 0; n<remainingItems; n++) {
    //     printf("n: %d\n", n);
    //     printf("i: %d\n", n%sizeOfInner + 1);
    //     printf("j: %d\n\n", n/sizeOfInner + sizeOfInner - remainingRows + 1);
    // }
    for(unsigned int thread=0; thread<threadCount; thread++) {
        startingRow = thread * rowsPerThread+1;
        endingRow = startingRow + rowsPerThread;
        for(unsigned int i=startingRow; i<endingRow; i++) {
            printf("Thread %u Row: %d\n", thread, i);
        }    
    }
    printf("\n");
    unsigned int i,j;
    for(unsigned int thread=0; thread<threadCount; thread++) {
        if(thread == (threadCount-1)) {
            startingRemainder = thread * remainingItemsMost;
            endingRemainder = startingRemainder + remainingItemsOutlier;
        } else {
            startingRemainder = thread * remainingItemsMost;
            endingRemainder = startingRemainder + remainingItemsMost;
        }
        for(unsigned int n=startingRemainder; n<endingRemainder; n++) {
            i = n%sizeOfInner + 1;
            j = n/sizeOfInner + sizeOfInner - remainingRows + 1;
            printf("Thread %u Row: %d\n", thread, j);
            printf("Thread %u Col: %d\n", thread, i);
        }
    }
    return 0;
}