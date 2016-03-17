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

// Hate globals
queue_t* directories;
queue_t* dirQueue;

subDir_t* createDirectory(const char* path, stack_t* files)
{
	subDir_t* newDir = malloc(sizeof(subDir_t));
	newDir->path = path;
	newDir->files = files;
	
	return newDir;
}


void* searchDirectory(void* path)
{
	// Directory to search
	const char* dir = (const char*) path;
	
	// clear and redo to open correctly
	char openDir[NAME_MAX] = "";
	sprintf(openDir, "./%s", dir);
	
	// Create streams and structs and vars and whatnots
  DIR *dp;
  struct dirent *ep;
	struct stat sb;
	stack_t* fileStack = malloc(sizeof(stack_t));

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
				else sprintf(dirName, "%s/%s", dir, ep->d_name);
				
				// Now check if it is a folder or a file
				if (stat(dirName, &sb) == 0 && S_ISDIR(sb.st_mode))
				{
					// We have a directory
					dirPrint(dirName);
					insertQueue(dirQueue, (void*) dirName);
				}
				else if (stat(dirName, &sb) == 0 && S_ISREG(sb.st_mode))
				{
					// We have file
					// Check if the file is executable by user
					if(sb.st_mode & S_IXUSR)
					{
						execPrint(dirName);
					}
					else 
					{
						push(fileStack, (void*) dirName);
						filePrint(dirName);
					}
				}
			} // End check directory
			else puts(ep->d_name);
		} // End directory stream

		// close the stream
		(void) closedir (dp);
		insertQueue(directories, createDirectory(dir, fileStack));
		// if we enable sub-directory search as well
	}
  else
    perror ("Couldn't open the directory");

	colorReset();
	
	return 0;
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************

int main (int argc, char* argv[])
{
	// Specify shallow or deep search, deep if true, default false
	bool searchType = false;
	if(argc != 2)
	{
		printf("Please specify if the search is shallow (only current directory)\n\
		or deep (sub-directories included)\n");
		exit(1);
	}
	else
	{
			if(strcasecmp(argv[1], "DEEP") == 0) searchType = true;
			else searchType = false;
	}
	
	// Time evaluations
	struct timeval StartTime, StopTime;
	unsigned int microsecs;
	int fileCount = 0, dirCount = 0;
	
	printf("Using %s search..\n", searchType ? "DEEP" : "SHALLOW");
	
	// initialize the main structure we are using 
	directories = malloc(sizeof(queue_t));
	dirQueue = malloc(sizeof(queue_t));
	
	// Use built-in function to properly initialize the structure
	queue_init(directories);
	queue_init(dirQueue);
	
	
	// search starts here, start recording
	gettimeofday(&StartTime, 0);
	
	// SEARCH BLOCK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	searchDirectory("");
	// Go deep if specified
	if(searchType)
	{
		itemQ_t* newSearch = removeQueue(dirQueue);
		while(newSearch != NULL)
		{
			searchDirectory(newSearch->keyValue);
			newSearch = removeQueue(dirQueue);
		}
	}
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
		//printf("Path: %s\n", temp->path);
		//printf("\tFiles(%d): \n", temp->files->size);
		fileCount += temp->files->size;
		
		// Pull out individual files
		itemS_t* someFiles = pop(temp->files);
		while(someFiles != NULL)
		{
			//printf("\t\t%s\n", (char (*)) (intptr_t) someFiles->keyValue);
			someFiles = pop(temp->files);
		}
		
		// Next subdirectory entry
		someDir = removeQueue(directories);
	}
	
	printf("\nFound %d files under %d directories in %u microseconds\n",
				 fileCount, dirCount, microsecs);
		
  return 0;
}
		
		