#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "dir_search.h"

// User libraries
#include "stack.h"
#include "queue.h"


//*****************************************************************************
//
// create a subdirectory struct
//
//*****************************************************************************

subDir_t* createDirectory(const char* path, STACK_t* files)
{
	subDir_t* newDir = malloc(sizeof(subDir_t*));
	newDir->path = path;
	newDir->files = files;
	
	return newDir;
}

//*****************************************************************************
//
// search a directory and append files to known path, subdirs to search in a queue
//
//*****************************************************************************
void* searchDirectory(void* dirStruct)
{
	// Directory to search
	mainDir_t* holdDirs = (mainDir_t*) dirStruct;
	// Extrapulate the necessary information
	const char* path = holdDirs->path;
	QUEUE_t* dirQueue = holdDirs->subDirQ;
	QUEUE_t* directories = holdDirs->baseDir;
	// clear and redo to open correctly
	char openDir[NAME_MAX] = "";
	// Sorry, disobeying const
	if(strcmp(path, "./") == 0) strcpy((char *) path, "");
	sprintf(openDir, "./%s", path);
	
	// Create streams and structs and vars and whatnots
  DIR *dp;
  struct dirent *ep;
	struct stat sb;
	STACK_t* fileStack = malloc(sizeof(STACK_t*));

	// Use built-in function to properly initialize the structure
	stack_init(fileStack);
	
	// Open directory
  dp = opendir ((const char*) openDir);
  if (dp != NULL)
	{
		while ((ep = readdir (dp)))
		{
			colorReset();	
			// If it isn't . or .. we have a valid directory to mark
			if(ep->d_name[0] != '.')
			{
				// initialize you fool!
				char* dirName = (char *) calloc(1, sizeof(char)*NAME_MAX);
				// Check if we are in main directory or a sub-directory
				if(!strcmp(path, "")) sprintf(dirName, "%s", ep->d_name);
				else sprintf(dirName, "%s/%s", path, ep->d_name);
				
				// Now check if it is a folder or a file
				if (stat(dirName, &sb) == 0 && S_ISDIR(sb.st_mode))
				{
					// We have a directory
					if(holdDirs->verbose) dirPrint(dirName);
					insertQueue(dirQueue, (void*) dirName);
				}
				else if (stat(dirName, &sb) == 0 && S_ISREG(sb.st_mode))
				{
					// We have file
					// Check if the file is executable by user
					if(sb.st_mode & S_IXUSR)
					{
						if(holdDirs->verbose) execPrint(dirName);
						free(dirName);
					}
					else 
					{
						if(holdDirs->verbose) filePrint(dirName);
						push(fileStack, (void*) dirName);
					}
				}
				else free(dirName);
			} // End check directory
			else if(holdDirs->verbose) puts(ep->d_name);
		} // End directory stream

		// close the stream
		(void) closedir (dp);
		insertQueue(directories, createDirectory(path, fileStack));
		// if we enable sub-directory search as well
	}
  else
    perror(path);

	colorReset();
	
	return 0;
}

//*****************************************************************************
//
// main search call
//
//*****************************************************************************

STACK_t* search(const char* path, bool recursive, bool verbose)
{
	// Time evaluations
	struct timeval StartTime, StopTime;
	QUEUE_t* directories;
	QUEUE_t* dirQueue;
	STACK_t* allFiles;
	unsigned int microsecs;
	int fileCount = 0, dirCount = 0;
	
	// initialize the main structure we are using 
	directories = malloc(sizeof(QUEUE_t*));
	dirQueue = malloc(sizeof(QUEUE_t*));
	allFiles = malloc(sizeof(STACK_t*));
	
	// Use built-in function to properly initialize the structure
	queue_init(directories);
	queue_init(dirQueue);
	stack_init(allFiles);
	
	// Construct a main directory structure
	mainDir_t mainDir = {path, verbose, directories, dirQueue};
	
	// search starts here, start recording
	gettimeofday(&StartTime, 0);
	
	// SEARCH BLOCK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	searchDirectory((void*) &mainDir);
	// Go deep if specified
	if(recursive)
	{
		itemQ_t* newSearch = removeQueue(dirQueue);
		while(newSearch != NULL)
		{
			mainDir.path = (const char*) (intptr_t) newSearch->keyValue;
			searchDirectory((void*) &mainDir);
			
			// Free the path and the subdirectory searched
			free(newSearch->keyValue);
			free(newSearch);
			newSearch = removeQueue(dirQueue);
		}
	}
	
	// Free the queue
	free(dirQueue);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	gettimeofday(&StopTime, 0);
	
	microsecs=((StopTime.tv_sec - StartTime.tv_sec)*1000000);
	dirCount = directories->size;

  if(StopTime.tv_usec > StartTime.tv_usec)
    microsecs+=(StopTime.tv_usec - StartTime.tv_usec);
  else
    microsecs-=(StartTime.tv_usec - StopTime.tv_usec);
	
	// Hold some stuff and things to print out entire search
	
	itemQ_t* someDir = removeQueue(directories);
	while(someDir != NULL)
	{
		// Pull out the subDir_t struct
		subDir_t* temp = (subDir_t*) (intptr_t) someDir->keyValue;
		fileCount += temp->files->size;
		
		// Pull out individual files
		itemS_t* someFiles = pop(temp->files);
		while(someFiles != NULL)
		{
			push(allFiles, someFiles->keyValue);
			free(someFiles);
			someFiles = pop(temp->files);
		}
		
		// Carefully free the stack pointer, the subdirectory pointer, and then
		// the directory in the queue
		free(temp->files);
		free(temp);
		free(someDir);
		// Next subdirectory entry
		someDir = removeQueue(directories);
	}
	
	free(directories);
	
	
	printf("\nFound %d files under %d directories in %u microseconds\n",
				 fileCount, dirCount, microsecs);
		
  return allFiles;
}