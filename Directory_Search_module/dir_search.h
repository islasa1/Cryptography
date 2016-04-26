#ifndef DIR_SEARCH
#define DIR_SEARCH

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#include "error_handler.h"
#include "stack.h"
#include "queue.h"

#define MAX_FILES 1000

// Pretty colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define colorReset() printf(ANSI_COLOR_RESET)
#define filePrint(str) printf(ANSI_COLOR_YELLOW "%s\n", str)
#define execPrint(str) printf(ANSI_COLOR_GREEN "%s\n", str)
#define dirPrint(str) printf(ANSI_COLOR_BLUE "%s\n", str)

typedef struct subDirectoryFiles
{
	const char* path;
	STACK_t* files;
} subDir_t;

typedef struct directorySearch
{
	const char* path;
	bool verbose;
	QUEUE_t* baseDir;
	QUEUE_t* subDirQ;
	// possibly implement a thread count here
} mainDir_t;

// Functions
subDir_t* createDirectory(const char* path, STACK_t* files);
void* searchDirectory(void* dirStruct);
STACK_t* search(const char* path, bool recursive, bool verbose);

#endif