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
	char userInput = '\0';
	bool success = false;
	scanf(" %c", &userInput);

	while(userInput != 'q')
	{
		file = NULL;
		myFiles = NULL;
		
		switch(userInput)
		{
			success = false;
			case '1':
				myFiles = search("", false, false);
				break;
			case '2':
				myFiles = search("", false, true);
				break;
			case '3':
				myFiles = search("..", true, false);
				break;
			case '4':
				myFiles = search("..", true, true);
				break;
			case '5':
				myFiles = search("../../../../..", true, false);
				break;
			case '6': 
				myFiles = search("EmptyFolder", false, false);
				break;
			case '7': 
				myFiles = search("/", false, false);
			default:
				break;
		}
		
		if(myFiles != NULL)
		{
			file = pop(myFiles);
			success = true;
		}
		else file = NULL;
		while(file != NULL)
		{
			free(file->keyValue);
			free(file);
			file = pop(myFiles);
		}
		free(myFiles);

		success ? printf("Success!\n") : printf("Fail!\n");
		
		scanf(" %c", &userInput);
	}
  
	return 0;
}