

#define	FALSE	0
#define TRUE	1
#define	LINE_LEN	512
#define MAX_ARGS	64
#define MAX_ARG_LEN	16
#define	MAX_PATHS	64
#define MAX_PATH_LEN	512
#define WHITESPACE	" ,\t\n"

#ifndef NULL
#define	NULL	0
#endif


struct command_t 
{
  char *name;
  int argc;
  char *argv[MAX_ARGS];
};


char *lookupPath(char **, char **);
int parseCommand(char *, struct command_t *);
int parsePath(char **);
void printPrompt();
void readCommand(char *);
void encryptFiles(const char*, bool, bool);
