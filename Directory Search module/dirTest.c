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
	stack_t* myFiles = NULL;
	int sum, avg, iter, diff;
	unsigned int oldCharAddr = 0;
	sum = avg = iter = diff = 0;
	
	while(1)
	{
		sleep(1);
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
		printf("Bytes Iter:%d\n\tDiff: %d\n\tSum: %d\n\tAvg: %d\n", iter, diff, sum, avg);
		
		for(int i = 0; i < 5; i++)
		{
			myFiles = search("", true);
			if(myFiles != NULL) file = pop(myFiles);
			else file = NULL;
			while(file != NULL)
			{
				free(file->keyValue);
				free(file);
				file = pop(myFiles);
			}
			if(myFiles != NULL) free(myFiles);
		}
		oldCharAddr = newCharAddr;
		iter+=5;
	}

	return 0;
}