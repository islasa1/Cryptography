#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "stack.h"
#include "dir_search.h"

// Test driver for search, soak test
	
int main(void)
{
	itemS_t* file = NULL;
	STACK_t* myFiles = NULL;
	int sum, avg, diff;
	unsigned long long iter;
	unsigned int oldCharAddr = 0;
	sum = avg = iter = diff = 0;
	
	while(1)
	{
		char* addr = calloc(1, 1);
		unsigned int newCharAddr = (unsigned int) (intptr_t) addr;
		free(addr);
		
		// Now do math
		if(iter == 0) diff = 0;
		else
		{
			diff = 	newCharAddr - oldCharAddr;
			sum += diff;
			avg = sum / iter; 
		}
		printf("Bytes Iter:%llu\n\tDiff: %d\n\tSum: %d\n\tAvg: %d\n", iter, diff, sum, avg);
		
		for(int i = 0; i < 5; i++)
		{
			myFiles = search("../../../../../", true, false);
			if(myFiles != NULL) file = pop(myFiles);
			else file = NULL;
			while(file != NULL)
			{
				free(file->keyValue);
				free(file);
				file = pop(myFiles);
			}
			free(myFiles);
		}
		oldCharAddr = newCharAddr;
		iter+=5;
		if(iter == 250) break;
	}
	return 0;
}