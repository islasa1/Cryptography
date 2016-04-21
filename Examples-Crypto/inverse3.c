#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// First calculate the Matrix of Minors, not actual Matrix of Minors
// it auto-implements cofactor as well since it takes less computation
// doing both at the same time
void minor3(float matrix[3][3], float minorMatrix[3][3])
{
	for(int i = 0; i < DIM; i++)
		for(int j = 0; j < DIM; j++)
			minorMatrix[i][j] = matrix[POSR0][POSC0]*matrix[POSR1][POSC1] - 
				matrix[POSR1][POSC0]*matrix[POSR0][POSC1];
}

// Now we can calculate the determinant easily
float det3(float matrix[3][3])
{
	float det = 0;
	float a[3];
	int row0, row1, col0, col1;
	row0 = 1;
	row1 = 2;
	
	for(int j = 0; j < DIM; j++)
		a[j] = matrix[row0][POSC0]*matrix[row1][POSC1] - 
			matrix[row1][POSC0]*matrix[row0][POSC1];
	
	for(int i = 0; i < DIM; i++)
			det += matrix[0][i] * a[i];
	
	return det;
}

// Use the transposed matrix (adjoint matrix) to find inverse
void transposeMatrix(float matrix[3][3])
{
	for(int i = 0; i < DIM; i++)
	{
		for(int j = 0; j < DIM; j++)
		{
			// Curse you destructive procedural C
			if(j > i) // Only do ~half the matrix
			{
				float temp = matrix[j][i];
				matrix[j][i] = matrix[i][j];
				matrix[i][j] = temp;
			}
		}
	}
}

bool inverse3(float matrix[3][3], float inverseMatrix[3][3])
{
	// Check determinant, if 0 abort inversion: not possible
	float det = det3(matrix);
	if(det == 0)
	{
		printf("Inversion not possible\n");
		return false;
	}
	
	// Clear the minor matrix just in case..
	for(int i = 0; i < DIM; i++)
		for(int j = 0; j < DIM; j++)
			inverseMatrix[i][j] = 0;
	
	// First calculate Matrix of Minors, with cofactor then transpose
	minor3(matrix, inverseMatrix);
	transposeMatrix(inverseMatrix);
	
	// Apply inverse determinant to adjoint matrix
	for(int i = 0; i < DIM; i++)
		for(int j = 0; j < DIM; j++)
			inverseMatrix[i][j] = inverseMatrix[i][j] / det;
		
	return true;
}