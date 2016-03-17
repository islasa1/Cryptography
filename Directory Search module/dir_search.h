#ifndef DIR_SEARCH
#define DIR_SEARCH

#include "stack.h"

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
	stack_t* files;
} subDir_t;

#endif