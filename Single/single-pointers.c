#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct
{
    double* left;
    double* right;
    double* up;
    double* down;
    double val;
    double pVal;
    unsigned long iterations;
} Node;

typedef struct
{
    double time_spent;
    unsigned long iterations;
} Result;

// TODO propper doc string
// Generates a 2D array of uninitialised Nodes 
Node** newPlane(unsigned int n) {
    Node** plane = ( Node** )malloc(n * sizeof(Node*));
    for (unsigned int i = 0; i < n; ++i)
        plane[i] = ( Node* )malloc(n * sizeof(Node));
    return plane;
}

// TODO propper doc string
// Populates the plane's wallswith the values provided, and sets the centre parts to zero. If debug is true then it prints outs the array
Node** populatePlane(Node** plane, unsigned int sizeOfPlane, int far_left, int far_right, int top, int bottom, bool debug)
{
     // Generate 2d array of nodes
    for(unsigned int j=0; j<sizeOfPlane; j++) {
        for(unsigned int i=0; i<sizeOfPlane; i++) {
            if(i == 0) {
                // Left
                plane[i][j].val = far_left;
                plane[i][j].pVal = far_left;
            } else if(j == 0) {
                // Top
                plane[i][j].val = top;
                plane[i][j].pVal = top;
            } else if(i == sizeOfPlane-1) {
                // Right
                plane[i][j].val = far_right;
                plane[i][j].pVal = far_right;
            } else if(j == sizeOfPlane-1) {
                // Bottom
                plane[i][j].val = bottom;
                plane[i][j].pVal = bottom;
            } else {
                plane[i][j].val = 0;
                plane[i][j].iterations = 0;
                plane[i][j].left = &plane[i-1][j].val;
                plane[i][j].right = &plane[i+1][j].val;
                plane[i][j].up = &plane[i][j+1].val;
                plane[i][j].down = &plane[i][j-1].val;
            }
        }
    }

    // Prints out the initial populated 2d array of Nodes
    if(debug) {
        printf("Original Array:\n");
        for(unsigned int j=0; j<sizeOfPlane; j++) {
         for(unsigned int i=0; i<sizeOfPlane; i++) {
             printf("%f, ", plane[i][j].val);
         }
         printf("\n");
     }
 }
 return plane;
}


// TODO propper doc string
// Runs the relaxation technique on the 2d array of Nodes that it is passed.
Result* relaxPlane(Node** plane, unsigned int sizeOfPlane, double tolerance, bool debug)
{
    clock_t begin = clock();
    while (1) {
        int flag = 0;
        for(unsigned int y=1; y<sizeOfPlane-1; y++) {
            for(unsigned int x=1; x<sizeOfPlane-1; x++) {
                plane[x][y].iterations ++;
                plane[x][y].pVal = plane[x][y].val;
                plane[x][y].val = (*plane[x][y].left + *plane[x][y].right + *plane[x][y].up + *plane[x][y].down)/4;
                if(flag==0 && tolerance < fabs(plane[x][y].val-plane[x][y].pVal)) {
                    flag = 1;
                }
            }
        }
        if(flag == 0) {
            break;
        }
    }
    clock_t end = clock();

    // Prints out the penultimate and final array if debug is true
    if(debug) {
        printf("Penultimate array\n");
        for(unsigned int j=0; j<sizeOfPlane; j++) {
            for(unsigned int i=0; i<sizeOfPlane; i++) {
                printf("%f, ", plane[i][j].pVal);
            }
            printf("\n");
        }
        printf("\n");
        // Print array of values
        printf("Final array\n");
        for(unsigned int j=0; j<sizeOfPlane; j++) {
            for(unsigned int i=0; i<sizeOfPlane; i++) {
                printf("%f, ", plane[i][j].val);
            }
            printf("\n");
        }
    }

    // Creates a Result struct with the ammount of time the relaxation technique took run, and the number of iterations it took
    Result* result = ( Result* ) malloc(sizeof(Result));
    result->time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    result->iterations = plane[1][1].iterations;

    return result;
}

int main(void)
{
    printf("%lu\n", sizeof(Node));
    bool debug = false;
    unsigned int sizeOfPlane = 100;
    double tolerance = 0.000000001;

    // Size of the plane must be at least 3x3
    if(sizeOfPlane < 3)
        sizeOfPlane = 3;
    // Tolerance must be greater than 0, or ends with exit code 1
    if(tolerance < 0)
        return 1;

    Node** plane = newPlane(sizeOfPlane);

    plane = populatePlane(plane, sizeOfPlane, 1, 1, 4, 4, debug);
    Result* result = relaxPlane(plane, sizeOfPlane, tolerance, debug);

    printf("Iterations: %lu\n", result->iterations);
    printf("Time Spent: %f\n", result->time_spent);
    return 0;
}