#include "utilities.h"

// Fisher-Yates shuffle
void shuffle(int *array, int size, int seed)
{
	int i, j;
	int temp;
	srand(seed);
	
	for(i = size; i > 0; i--)
	{
		j = rand() % i;
		temp = array[j];
		array[j] = array[i-1];
		array[i-1] = temp;
	}
}
