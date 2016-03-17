#ifndef HILLCIPHER_H
#define HILLCIPHER_H

#include <stdbool.h>

#define Z_PRIME (251)
#define DIM 3
#define POSR0 ((i + 1) % DIM)
#define POSR1 ((i + 2) % DIM)
#define POSC0 ((j + 1) % DIM)
#define POSC1 ((j + 2) % DIM)

#ifdef USE_DEF_KEY2
	const unsigned int default_key[2][2] = {{26, 105}, {3, 67}};
	const unsigned int default_keyInverse[2][2] = {{104, 103}, {89, 149}};
#endif

// Encrypt a 2x1 matrix (stream of chars by 2's) with a 2x2 key
void hillcipher2(unsigned int inputStream[2][1], unsigned int outputStream[2][1], const unsigned int key[2][2]);

// Check the integrity of key (is inverse correct)
bool checkKeyIntegrity2(const unsigned int key[2][2], const unsigned int key_inverse[2][2]);
bool checkKeyIntegrity3(const unsigned int key[3][3], const unsigned int key_inverse[3][3]);

unsigned int convertZ(int, unsigned int);
bool inverse2_Z(unsigned int input[2][2], unsigned int output[2][2]);
bool inverse3_Z(unsigned int input[3][3], unsigned int output[3][3]);

// useful for inverse3_Z()
int det3(unsigned int matrix[3][3]);
void minor3(unsigned int matrix[3][3], unsigned int minorMatrix[3][3]);
void transposeMatrix(int matix[3][3]);

#endif
