#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(void)
{   
    unsigned int sizeOfPlane = 64;
    // unsigned int threadCount = 3;
    unsigned int i,j;

    unsigned int sizeOfInner = sizeOfPlane-2;
    unsigned int remaindingItemsPR = sizeOfInner%8;
    unsigned int iMax = sizeOfPlane-1;
    unsigned int jMax = iMax-remaindingItemsPR;

    printf("Size of Inner: %d\n", sizeOfInner);
    printf("Reamind Items per Row: %d\n", remaindingItemsPR);


    for(i=1;i<iMax; i++){
        for(j=1; j<jMax; j++) {
            if(j%2==i%2){
                printf("x");
            } else {
                printf("o");
            }
        }
        for(j=jMax; j<iMax; j++) {
            printf("r");
        }
        printf("\n");
    }
    return 0;
}