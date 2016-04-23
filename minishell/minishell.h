#ifndef MINISHELL_H
#define MINISHELL_H

#define FALSE		0
#define TRUE		1
#define LINE_LEN	512
#define MAX_ARGS	64
#define MAX_ARG_LEN	16
#define	MAX_PATHS	64
#define MAX_PATH_LEN	512
#define WHITESPACE	" ,\t\n"
#define ASCII_RANGE  94 
#define ASCII_BASE   32
#define MAX_THREADS	1

#ifndef NULL
#define	NULL	0
#endif

#include <sys/types.h>

struct command_t 
{
  char *name;
  int argc;
  char *argv[MAX_ARGS];
};

struct crypt_thread_args_t
{
	FILE *inputFile, *outputFile;
	char *fileName, *newFile, *tag;
	unsigned int key[2][2];
	unsigned char threadNum;
	bool mode, stats;
	unsigned int *microsecs;
	long int *bytes;
	int *skippedFiles;
};


char * lookupPath(char **, char **);
int parseCommand(char *, struct command_t *);
int parsePath(char **);
void printPrompt();
void readCommand(char *);
void encryptFiles(const char *, bool, bool, bool, bool);
void decryptFiles(const char *, bool, bool, bool, bool);
bool getKey(unsigned int[2][2]);
bool tagFile(FILE *, char *);
bool checkTag(FILE *, char *);
void * threadedEncrypt(void *);
void * threadedDecrypt(void *);

#endif