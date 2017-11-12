#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(void)
{   
    unsigned int sizeOfPlane = 10;
    unsigned int threadCount = 3;
    // unsigned int threadID = 1;

    unsigned int sizeOfInner = sizeOfPlane-2;
    unsigned int rowsPerThreadS = sizeOfInner/threadCount+1;
    unsigned int rowsPerThreadE = sizeOfInner/threadCount;
    unsigned int remainingRows = sizeOfInner - threadCount*rowsPerThreadE;

    unsigned int startingRow;
    unsigned int endingRow;

    printf("Size of Inner: %d\n", sizeOfInner);
    printf("Rows per thread Start: %d\n", rowsPerThreadS);
    printf("Rows per thread End: %d\n", rowsPerThreadE);
    printf("Remainding Rows: %d\n", remainingRows);
    for(unsigned int thread = 0; thread<threadCount; thread++) {
        if(thread < remainingRows) {
            startingRow = thread * rowsPerThreadS + 1;
            endingRow = startingRow + rowsPerThreadS;
        } else {
            startingRow = thread * rowsPerThreadE + remainingRows + 1;
            endingRow = startingRow + rowsPerThreadE;
        }
        for(unsigned int i=startingRow; i<endingRow; i++) {
            printf("Thread %u Row: %d\n", thread, i);
        }    
    }
    return 0;
}