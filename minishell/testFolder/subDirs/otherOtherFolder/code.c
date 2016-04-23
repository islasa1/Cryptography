#include "utilities.h"

// Fisher-Yates shuffle
void shuffle(int *array, int size, int seed)
{
	// We need to use reentrantly-safe pseudo-random number generators
	// for portabillity use with any threaded/nonthreaded program
	struct drand48_data *randData = NULL;
	srand48_r((long int) seed, randData);
	int i, j;
	int temp;
	long int result;
	
	for(i = size; i > 0; i--)
	{
		lrand48_r(randData, &result);
		j =  result % i;
		temp = array[j];
		array[j] = array[i-1];
		array[i-1] = temp;
	}
}
islasa1