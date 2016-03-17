#ifndef INVERSE3_H
#define INVERSE3_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DIM 3
#define POSR0 ((i + 1) % DIM)
#define POSR1 ((i + 2) % DIM)
#define POSC0 ((j + 1) % DIM)
#define POSC1 ((j + 2) % DIM)


float det3(float matrix[3][3]);
void minor3(float matrix[3][3], float minorMatrix[3][3]);
void transposeMatrix(float matix[3][3]);
bool inverse3(float matrix[3][3], float inverseMatrix[3][3]);

#endif