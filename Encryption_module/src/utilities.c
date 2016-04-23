#include "utilities.h"

// Fisher-Yates shuffle
void shuffle(int *array, int size, int seed)
{
	// Our "seed"
	unsigned int *seedp = ((unsigned int*) &seed);
	int i, j;
	int temp;
	
	for(i = size; i > 0; i--)
	{
		// Note: We don't care about how "weak" rand_r() may be since we mod anyhow
		j =   rand_r(seedp) % i;
		temp = array[j];
		array[j] = array[i-1];
		array[i-1] = temp;
	}
}
