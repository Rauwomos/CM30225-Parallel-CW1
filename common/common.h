#ifndef COMMON_H
#define COMMON_H
#include <time.h>
#include <stdbool.h>

struct arguments
{
    unsigned int sizeOfPlane;  // 10
    double tolerance;  // 0.00001
    double left;  // 4
    double right;  // 2
    double top;  // 1
    double bottom;  // 3
    bool debug;  // false
    bool valid;  // true
};


long double toSeconds(struct timespec start, struct timespec end);

// Generates a 2D array of uninitialised doubles
double** newPlane(unsigned int n);

// // Generates a 2D array of uninitialised doubles in a single contiguous block of memory
// double* newPlaneContiguous(unsigned int n);

// Populates the plane's wallswith the values provided, and sets the centre parts to zero
double** populatePlane(double** plane, unsigned int sizeOfPlane, double top, double bottom, double farLeft, double farRight);

// // Populates the plane's wallswith the values provided, and sets the centre parts to zero
// double** populatePlaneContiguous(double* plane, unsigned int sizeOfPlane, double top, double bottom, double farLeft, double farRight);

void printPlane(double** plane, unsigned int sizeOfPlane);

// void printPlaneContiguous(double* plane, unsigned int sizeOfPlane);

// Parse command-line arguments
struct arguments parseArguments(int argc, char **argv);

#endif /* COMMON_H */