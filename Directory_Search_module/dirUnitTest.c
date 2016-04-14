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
    if(myFiles != NULL) free(myFiles);
  }
	return 0;
}