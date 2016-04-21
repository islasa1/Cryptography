#include "hillcipher.h"

#define DIM 3
#define POSR0 ((i + 1) % DIM)
#define POSR1 ((i + 2) % DIM)
#define POSC0 ((j + 1) % DIM)
#define POSC1 ((j + 2) % DIM)

//****************************************************************
//
// Encrypt a 2x1 matrix (stream of chars by 2's) with a 2x2 key
//
//****************************************************************
void hillcipher2(unsigned int inputStream[2][1], unsigned int outputStream[2][1], const unsigned int key[2][2])
{
	if(key == NULL) return;
	// Rows of first matrix	
	
	int i, k;

	// clear the buffer
	ERROR_PTR_VOID(memset(outputStream, 0, sizeof(unsigned int) * 2));

	for(i = 0; i < 2; i++)
	{
		// Inner number for multiplication
		for(k = 0; k < 2; k++)
		{
			outputStream[i][0] += key[i][k] * inputStream[k][0];
			outputStream[i][0] %= Z_PRIME;
		}
	}
	// Return the encrypted chars
	return;
}

//************************************************************
//
// Check the integrity of 2x2 key (does it have inverse?)
//
//************************************************************
bool checkKeyIntegrity2(const unsigned int key[2][2], const unsigned int keyInverse[2][2])
{
	unsigned int check[2][2] = {{0, 0}, {0, 0}};
	int i, j, k;
	
	// Rows of first matrix
	for(i = 0; i < 2; i++)
	{
		// columns of second matrix
		for(j = 0; j < 2; j++)
		{
			// Inner number for multiplication
			for(k = 0; k < 2; k++)
			{
				check[i][j] += keyInverse[i][k] * key[k][j];
				check[i][j] %= Z_PRIME;
			}
		
			// check if we created an identity matrix
			if( i != j && (check[i][j] != 0))  return false;
			else if(i == j && (check[i][j] != 1)) return false;
		}
	}

	// If we haven't returned by now it is an I(n) matrix
	return true;

	
}

//************************************************************
//
// Calculate number for Z_PRIME field
//
//************************************************************
unsigned int convertZ(int numerator, long int denominator)
{
	// Magic
	int x = 1;
	denominator %= Z_PRIME;
	if(denominator < 0) denominator += Z_PRIME;
	long int converter = denominator;
	// Prevent overflow
	while(converter != 1)
	{
		x++;
		converter = (denominator*x % Z_PRIME);
	}
	
	if(numerator < 0) numerator += Z_PRIME;
	return (unsigned int) (numerator * x % Z_PRIME);
}

//************************************************************
//
// Calculate inverse of a 2x2 matrix
//
//************************************************************
bool inverse2_Z(unsigned int input[2][2], unsigned int output[2][2])
{
	long int det = (long int) input[0][0]*input[1][1] - (long int) input[0][1]*input[1][0];
	if(det == 0) return false;
	
	unsigned int hold = input[0][0];
	output[0][0] = input[1][1];
	output[0][1] = convertZ(-input[0][1], 1);
	output[1][0] = convertZ(-input[1][0], 1);
	output[1][1] = hold;
	
	// now apply inverse determinant	
	int i, j;
	for(i = 0; i < 2; i++)
		for(j = 0; j < 2; j++)
			output[i][j] = convertZ( output[i][j], det);
	
	return true;
	
}
