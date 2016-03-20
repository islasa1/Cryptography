/* 
 * This is a very minimal shell.  It is only required to find an executable
 * in the PATH, then load it and execute it (using execv).  Since it uses
 * "." (dot) as a separator, it cannot handle file names like "minishell.h"
 * The focus on this exercise is to use fork, PATH variables, and exec.
 * This code can be extended by doing the exercise at the end of Chapter 9.
 *
 * Provided as a solution to Kernel Projects by Gary Nutt with contributions
 * by Sam Siewert and Panos Tsirigotis
 *
 * Modified by Sam Siewert (2013) to remove gets() call considered dangerous and
 * replaced with recommended fgets with line-feed stripping.
 *
 * Modifed by Anthony Islas (2016) to implement cd bash command and allow for 
 * use of escape character '\' to pass whitespace ' ' into an argument 
 * e.g. cd Dir\ Name -> argv[0] = cd, argv[1] = Dir Name
 */
#include	<stdio.h>
#include	<stdlib.h>
#include    <stdbool.h>
#include	<sys/types.h>
#include    <sys/stat.h>
#include	<sys/wait.h>
#include	<string.h>
#include	<unistd.h>
#include    <errno.h>

// Necessary to declare before encryption library
#define USE_DEF_KEY2

// User libraries
#include	"minishell.h"
#include    "login.h"
#include    "stack.h"
#include    "dir_search.h"
#include    "hillcipher.h"
#include    "TSHencrypt.h"

extern const unsigned int default_key[2][2];
extern const unsigned int default_keyInverse[2][2];

char promptString[LINE_LEN] = " > ";
extern FILE *stdin;

int main() 
{
    char commandLine[LINE_LEN];
    char *pathv[MAX_PATHS];
    int i;
    int chPID;		// Child PID
    int stat;		// Used by parent wait
    bool loggedIn = false; // Check to see if user is logged into system
    pid_t thisChPID;
    pid_t finishedChPID;
    struct command_t command;

    // Shell initialization
    for(i=0; i<MAX_ARGS; i++)
        command.argv[i] = (char *) malloc(MAX_ARG_LEN);

    parsePath(pathv);

    // Main loop
    while(TRUE) 
    {
        printPrompt();

        // Read the command line and parse it
        readCommand(commandLine);

        if( (strcmp(commandLine, "exit") == 0) ||
            (strcmp(commandLine, "quit") == 0))
        {
            break;	// Quit the shell
        }
        if(commandLine[0] == '\0') continue;

        parseCommand(commandLine, &command);

#ifdef DEBUG
        printf("... returned from parseCommmand ...\n");

        for(i=0; i<command.argc; i++)
            printf("	argv[%d] = %s\n", i,  command.argv[i]);
#endif
        
        //*********************************************************************************************************************************
        // Check for built in commands to override UNIX commands lookup 
        if(strcmp(command.argv[0], "cd") == 0)
        {
            int err = 0;
            // Use chdir to do cd bash function
            if(command.argc == 2)
                err = chdir(command.argv[1]);
            else printf("Please specify a directory\n");
            
            if(err < 0) perror("Failed to open directory\n");  
            
            continue;
        }
        else if(strcmp(command.argv[0], "login") == 0)
        {
            // Check next arguments for what to do
            if(command.argc < 2)
            {
                printf("login: Please specify if logging in as existing user -l, or new user -n\n"); 
                continue;
            }
            else if(strcmp(command.argv[1], "-l") == 0)
            {
                // Login protocol standard user
                loggedIn = loginProtocol('l');
            }
            else if(strcmp(command.argv[1], "-n") == 0)
            {
                // Login protocol new user
                loggedIn = loginProtocol('n');
            }
            else
            {
                printf("login: Please specify if logging in as existing user -l, or new user -n\n");    
                continue;
            }
            
            // See if we logged in, MAX_NAME defined in login.h
            if(loggedIn)
            {
                char username[MAX_NAME];
                loginGetUsername(username);
                sprintf(promptString, "%s > ", username);
            }
            else sprintf(promptString, " > ");
            
            continue;
        }
        else if(strcmp(command.argv[0], "su") == 0)
        {
            bool previousLoggedIn = loggedIn;
            int cur_user = loginGetCurUser();
            loggedIn = loginProtocol('l'); 
            if(loggedIn)
            {
                char username[MAX_NAME];
                loginGetUsername(username);
                sprintf(promptString, "%s > ", username);
            }
            else if(previousLoggedIn) 
            {
                loginSetCurUser(cur_user);
                loggedIn = true;
            }
            
            continue;
        }
        else if(strcmp(command.argv[0], "encrypt") == 0)
        {
            if(loggedIn)
            {
                bool error = false;
                if(command.argc == 2)
                {
                    // Do base directory search, non-verbose
                    encryptFiles(command.argv[1], false, false);
                }
                else if(command.argc == 3)
                {
                    // are we doing verbose or recursive option?
                    if(strcmp(command.argv[1], "-r") == 0) encryptFiles(command.argv[2], true, false);
                    else if(strcmp(command.argv[1], "-v") == 0) encryptFiles(command.argv[2], false, true);
                    else error = true;
                }
                else if(command.argc == 4)
                {
                    // verify correct input of verbose and recursive flags
                    if((strcmp(command.argv[1], "-r") == strcmp(command.argv[2], "-v")) || 
                       (strcmp(command.argv[1], "-v") == strcmp(command.argv[2], "-r")))
                        encryptFiles(command.argv[3], true, true);
                    else error = true;
                }
                else error = true;
                if(error) printf("encrypt: Please specify a file or directory (, with optional flags beforehand to encrypt\n\t\
                \rsubdirectories (recursive -r) and/or show files and directories found (verbose -v) e.g encrypt -r -v foo.txt\n"); 
            }
            else printf("ms: Must be logged in as user to encrypt\n");
            
            continue;
        }
        else if(strcmp(command.argv[0], "decrypt") == 0)
        {
            if(loggedIn)
            {
                bool error = false;
                if(command.argc == 2)
                {
                    // Do base directory search, non-verbose
                    decryptFiles(command.argv[1], false, false);
                }
                else if(command.argc == 3)
                {
                    // are we doing verbose or recursive option?
                    if(strcmp(command.argv[1], "-r") == 0) decryptFiles(command.argv[2], true, false);
                    else if(strcmp(command.argv[1], "-v") == 0) decryptFiles(command.argv[2], false, true);
                    else error = true;
                }
                else if(command.argc == 4)
                {
                    // verify correct input of verbose and recursive flags
                    if((strcmp(command.argv[1], "-r") == strcmp(command.argv[2], "-v")) || 
                       (strcmp(command.argv[1], "-v") == strcmp(command.argv[2], "-r")))
                        decryptFiles(command.argv[3], true, true);
                    else error = true;
                }
                else error = true;
                if(error) printf("decrypt: Please specify a file or directory (, with optional flags beforehand to decrypt\n\t\
                \rsubdirectories (recursive -r) and/or show files and directories found (verbose -v) e.g decrypt -r -v foo.txt\n"); 
            }
            else printf("ms: Must be logged in as user to decrypt\n");
            
            continue;
        }
        
        // End of built in shell commands
        //*********************************************************************************************************************************

        // Get the full pathname for the file
        command.name  = lookupPath(command.argv, pathv);

#ifdef DEBUG
        printf("... returned from lookupPath ...\n");
        printf("	command path = %s\n", command.name);
        for(i=0; i<command.argc; i++)
	    printf("	argv[%d] = %s\n", i,  command.argv[i]);
#endif

        if(command.name == NULL) 
        {
            fprintf(stderr, "Command %s not found\n", command.argv[0]);
            continue;
        }

        // Create a process to execute the command
        if((chPID = fork()) == 0) 
        {
            //  This is the child, that will execute the command requested

#ifdef DEBUG
            printf("child executing: %s\n", command.name);
            for(i=1; i<command.argc; i++)
	            printf("	argv[%d] = %s\n", i,  command.argv[i]);
#endif

            execv(command.name, command.argv);
        }
        
        // Wait for the child to terminate

#ifdef DEBUG
       printf("Parent waiting\n");
#endif

        finishedChPID = thisChPID = wait(&stat);
        
        if(finishedChPID == -1) printf("Error: child termination error\n");
    }

// Shell termination

}


char *lookupPath(char **argv, char **dir) 
{

    // This function inspired by one written by Sam Siewert in Spring 1996
    int i;
    char *result;
    char pName[MAX_PATH_LEN];

    // Check to see if file name is already an absolute path name
    if(*argv[0] == '/') 
    {
        result = (char *) malloc(strlen(argv[0])+1);
        strcpy(result, argv[0]);
        return result;
    }

    // Look in PATH directories
    // This code does not handle the case of a relative pathname
    for(i = 0; i < MAX_PATHS; i++) 
    {
        if(dir[i] == NULL) break;
        strcpy(pName, dir[i]);
        strcat(pName, "/");
        strcat(pName, argv[0]);

#ifdef DEBUG
        printf("lookupPath: Checking for %s\n", pName);
#endif

        if(access(pName, X_OK | F_OK) != -1) 
        {

            // File found
#ifdef DEBUG
            printf("lookupPath: Found %s in %s (full path is %s)\n",
			argv[0], dir[i], pName);
#endif
            result = (char *) malloc(strlen(pName)+1);
            strcpy(result, pName);
            return result;		// Return with success
        }
    }

    // File name not found in any path variable
    fprintf(stderr, "%s: command not found\n", argv[0]);
    return NULL;

}


int parseCommand(char *cLine, struct command_t *cmd) 
{
    // Determine command name and construct the parameter list
    int argc;
    char **clPtr;

    // Initialization
    clPtr = &cLine;
    argc = 0;

    // Get the command name and parameters
    // This code does not handle multiple WHITESPACE characters
    while((cmd->argv[argc++] = strsep(clPtr, WHITESPACE)) != NULL) ;
    
    // Modified code to allow escape characters ONLY FOR ' ' (spaces)
    // Any technical WHITESPACE escaped is assumed ' '
    int count = 0;
    int holdArgc = argc;
    while(count < argc - 1)
    {
        if((cmd->argv[count])[strlen(cmd->argv[count]) - 1] == '\\')
        {
#ifdef DEBUG
            printf("Found escape character\n"); 
#endif 
            if(count < holdArgc - 2)
            {
                (cmd->argv[count])[strlen(cmd->argv[count]) - 1] = ' ';
                strcat(cmd->argv[count], cmd->argv[count+1]);
                argc--;
            }
        }
        count++;
    }

    cmd->argv[argc--] = '\0';	// Null terminated list of strings
    cmd->argc = argc;

    return  1;	
}


int parsePath(char *dirs[]) 
{
    // This routine based on one written by Panos Tsirigotis, Spring 1989
    int i;
    char *pathEnvVar;
    register char *thePath, *oldp;

    for(i=0; i<MAX_ARGS; i++)
	dirs[i] = NULL;

    pathEnvVar = (char *) getenv("PATH");
    thePath = (char *) malloc(strlen(pathEnvVar) + 1);
    strcpy(thePath, pathEnvVar);

    i = 0;
    oldp = thePath;

    for(;; thePath++) 
    {
	if((*thePath == ':') || (*thePath == '\0')) 
        {
            dirs[i] = oldp;
            i++;

            if(*thePath == '\0') break;

            *thePath = '\0';
            oldp = thePath + 1;
        }
    }

#ifdef DEBUG
    printf("Directories in PATH variable\n");
        for(i=0; i<MAX_PATHS; i++)
            if(dirs[i] != '\0') printf("	Directory[%d]: %s\n", i, dirs[i]);
#endif
    
    return 1;

}


void printPrompt() 
{
    printf("%s ", promptString);
}

void readCommand(char *buffer) 
{
    // original dangerous stdio call - can over-run buffer, see man gets
    // gets(buffer); 

  
    // suggested safer replacement call - can't go beyond length provided,
    // but we must strip off the line feed included in the buffer unlike gets
    // 
    fgets(buffer, LINE_LEN, stdin);

    buffer[strlen(buffer)-1] = '\0';  // overwrite the line feed with null term
}


void encryptFiles(const char* file, bool recursive, bool verbose)
{
    struct stat sb;
    bool pathLocal = false; // Did we just get path from file (single file) or dynamically from search?
    STACK_t* files = NULL;
    char checkDir[MAX_NAME] = "";
    
    // check to see if it is the home directory
    if(strcmp(file, "./") == 0) strcpy(checkDir, "./");
    else sprintf(checkDir, "./%s", file);
    
    // Make it so all inputs are based on base directory
    
    
    // Key gen
    //! TODO: Implement key gen unique to each user
    
    // Check if it is a folder or a file
    if (stat(checkDir, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        // We have a directory
        files = search(file, recursive, verbose);
    }
    else if (stat(checkDir, &sb) == 0 && S_ISREG(sb.st_mode))
    {
        // We have file
        // Check if the file is executable by user
        if(sb.st_mode & S_IXUSR)
        {
            // Is exec
            printf("encrypt: Program will not encrypt executables\n");
        }
        else 
        {
            // Is file
            if(recursive) printf("encrypt: warning: files cannot be recursively searched\n");
            if(verbose)
            {
                filePrint(file);
                colorReset();
            }
            files  = malloc(sizeof(STACK_t*));
            stack_init(files);
            
            // Push single file into stack
            push(files, (void*) file);
            pathLocal = true;
        }
    }
    else
    {
        printf("encrypt: Could not encrypt. Argument to encrypt is neither file or directory\n");
        return;
    }
    
    
    // Confirm we have files to encrypt and proceed
    if(files != NULL  && files->head != NULL)
    {
        char inputBuffer[LINE_LEN];
        int totalFiles = files->size;
        printf("%d files will be encrypted, each file output as file.ext.crpt\n\tProceed? [Yn]: ", totalFiles);
        readCommand(inputBuffer);
        if(inputBuffer[0] != '\0' && (inputBuffer[0] == 'y' || inputBuffer[0] == 'Y'))
        {
            // Encrypt the files!!! 
            int skippedFiles = 0;
            int encryptFiles = 0;
            itemS_t* curFile = pop(files);
            while(curFile != NULL)
            {
                char newFile[MAX_NAME] = "";
                char* fileName = CHAR_PTR curFile->keyValue;
                
                char* crptExt = strrchr(fileName, '.');
                
                // Yay, pointer math
                if(crptExt != NULL && (strcmp(crptExt, ".crpt") != 0))
                {
                    // Valid file to encrypt
                    sprintf(newFile, "%s.crpt", fileName);
                    encryptFiles++;
                }
                else
                {
                    if(!pathLocal) free(curFile->keyValue);
                    free(curFile);

                    curFile = pop(files);
                    continue;
                }
                
                FILE *inputFile, *outputFile;
                if((inputFile = fopen(fileName, "r+")) == NULL)
                {
                    printf("File not found\n");
                    skippedFiles++;
                    continue;
                };
                if((outputFile = fopen(newFile, "w+")) == NULL)
                {
                    printf("Could not open output file\n");
                    skippedFiles++;
                    continue;
                }
                encrypt(inputFile, outputFile, default_key);
                
                fclose(inputFile);
                fclose(outputFile);
                
                remove(fileName);
                
                if(!pathLocal) free(curFile->keyValue);
                free(curFile);
                
                curFile = pop(files);
            }
            
            
            printf("Encrypted %d of %d total files\n", encryptFiles - skippedFiles, totalFiles);
            printf("Encrypted %d of %d total non-encrypted files\n", encryptFiles - skippedFiles, encryptFiles);
            printf("Skipped %d of %d total non-encrypted files\n", skippedFiles, encryptFiles);
        }
        else 
        {
            printf("encrypt: Encryption aborted\n");
            // Clear stack
            clearStack(files, pathLocal);
        }
    }
    else printf("encrypt: No files to encrypt\n");
}

void decryptFiles(const char* file, bool recursive, bool verbose)
{
    struct stat sb;
    bool pathLocal = false; // Did we just get path from file (single file) or dynamically from search?
    STACK_t* files = NULL;
    char checkDir[MAX_NAME] = "";
    
    // check to see if it is the home directory
    if(strcmp(file, "./") == 0) strcpy(checkDir, "./");
    else sprintf(checkDir, "./%s", file);
    
    // Make it so all inputs are based on base directory
    
    
    // Key gen
    //! TODO: Implement key gen unique to each user
    
    // Check if it is a folder or a file
    if (stat(checkDir, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        // We have a directory
        files = search(file, recursive, verbose);
    }
    else if (stat(checkDir, &sb) == 0 && S_ISREG(sb.st_mode))
    {
        // We have file
        // Check if the file is executable by user
        if(sb.st_mode & S_IXUSR)
        {
            // Is exec
            printf("decrypt: Program will not decrypt executables\n");
        }
        else 
        {
            // Is file
            if(recursive) printf("decrypt: warning: files cannot be recursively searched\n");
            if(verbose)
            {
                filePrint(file);
                colorReset();
            }
            files  = malloc(sizeof(STACK_t*));
            stack_init(files);
            
            // Push single file into stack
            push(files, (void*) file);
            pathLocal = true;
        }
    }
    else
    {
        printf("decrypt: Could not decrypt. Argument to decrypt is neither file or directory\n");
        return;
    }
    
    
    // Confirm we have files to encrypt and proceed
    if(files != NULL  && files->head != NULL)
    {
        char inputBuffer[LINE_LEN];
        int totalFiles = files->size;
        printf("%d files will be decrypted, each file output as file.ext - .crpt\n\tProceed? [Yn]: ", totalFiles);
        readCommand(inputBuffer);
        if(inputBuffer[0] != '\0' && (inputBuffer[0] == 'y' || inputBuffer[0] == 'Y'))
        {
            // Encrypt the files!!! 
            int skippedFiles = 0;
            int decryptFiles = 0;
            itemS_t* curFile = pop(files);
            while(curFile != NULL)
            {
                char newFile[MAX_NAME] = "";
                char* fileName = CHAR_PTR curFile->keyValue;
                char* crptExt = strrchr(fileName, '.');
                
                // Yay, pointer math
                if(crptExt != NULL && (strcmp(crptExt, ".crpt") == 0))
                {
                    // Valid file to decrypt
                    // +1 for '\0'
                    strncpy(newFile, fileName, (abs(crptExt - fileName)));
                    newFile[abs(crptExt - fileName)] = '\0';
                    decryptFiles++;
                }
                else
                {
                    if(!pathLocal) free(curFile->keyValue);
                    free(curFile);
                    curFile = pop(files);
                    continue;
                }
                
                FILE *inputFile, *outputFile;
                if((inputFile = fopen(CHAR_PTR curFile->keyValue, "r+")) == NULL)
                {
                    printf("File not found\n");
                    skippedFiles++;
                    continue;
                };
                if((outputFile = fopen(newFile, "w+")) == NULL)
                {
                    printf("Could not open output file\n");
                    skippedFiles++;
                    continue;
                }
                decrypt(inputFile, outputFile, default_key);
                
                fclose(inputFile);
                fclose(outputFile);
                
                remove(fileName);
                
                
                if(!pathLocal) free(curFile->keyValue);
                free(curFile);
                
                curFile = pop(files);
            }
            
            printf("Decrypted %d of %d total files\n", decryptFiles - skippedFiles, totalFiles);
            printf("Decrypted %d of %d total encrypted files\n", decryptFiles - skippedFiles, decryptFiles);
            printf("Skipped %d of %d total encrypted files\n", skippedFiles, decryptFiles);
        }
        else 
        {
            printf("decrypt: Encryption aborted\n");
            // Clear stack
            clearStack(files, pathLocal);
        }
    }
    else printf("decrypt: No files to decrypt\n");
}
