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

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include	<sys/types.h>
#include    <sys/time.h>
#include    <sys/stat.h>
#include	<sys/wait.h>
#include	<string.h>
#include	<unistd.h>
#include    <errno.h>

// Necessary to declare before encryption library
#define USE_DEF_KEY2

// User libraries
#include    "error_handler.h"
#include    "minishell.h"
#include    "login.h"
#include    "stack.h"
#include    "dir_search.h"
#include    "hillcipher.h"
#include    "TSHencrypt.h"

#include    "shell_utils.c"

extern const unsigned int default_key[2][2];
extern const unsigned int default_keyInverse[2][2];
extern FILE *stdin;

char promptString[LINE_LEN] = " > ";


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
    if(!LoginModuleInit()) 
    {
        printf("\nms: Could not find or create passphrase file\n");
        exit(-1);   
    }
    for(i=0; i<MAX_ARGS; i++) 
	{
        command.argv[i] = (char *) malloc(MAX_ARG_LEN);
		if(command.argv[i] == NULL) 
		{
			printf("\nms: Failed to allocate memory\n");
			exit(-1);
		}
	}

    if(parsePath(pathv) == -1)
	{
		printf("\nms: Failed to parse path");
		exit(-1);
	}

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

        ERROR_NUM_NUM(parseCommand(commandLine, &command));    
        //*********************************************************************************************************************************
        // Check for built in commands to override UNIX commands lookup 
        if(strcmp(command.argv[0], "cd") == 0)
        {
            int err = 0;
            // Use chdir to do cd bash function
            if(command.argc == 2)
                err = chdir(command.argv[1]);
            else printf("Please specify a directory\n");
            
            if(err < 0) perror("Failed to open directory: ");  
            
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
                char* username = loginGetUsername();
                sprintf(promptString, "%s > ", username);
            }
            else sprintf(promptString, " > ");
            
            continue;
        }
        else if(strcmp(command.argv[0], "logout") == 0)
        {
            if(loggedIn)
            {
                sprintf(promptString, " > ");
                loginSetCurUser(-1);
                loggedIn = false;
                continue;
            }
            else 
            {
                printf("ms: Cannot logout. Not logged in\n");
                continue;
            }
        }
        else if(strcmp(command.argv[0], "su") == 0)
        {
            bool previousLoggedIn = loggedIn;
            int cur_user = loginGetCurUser();
            loggedIn = loginProtocol('l'); 
            if(loggedIn)
            {
                char* username = loginGetUsername();
                sprintf(promptString, "%s > ", username);
            }
            else if(previousLoggedIn) 
            {
                loginSetCurUser(cur_user);
                loggedIn = true;
            }
            
            continue;
        }
        else if(strcmp(command.argv[0], "encrypt") == 0 || strcmp(command.argv[0], "decrypt") == 0)
        {
            if(loggedIn)
            {
                bool error = false;
                int argPaths[MAX_ARGS], j = 0;
                bool recursive = false, verbose = false, mode = MODE_TEXT, stats = false;

                memset(argPaths, 0, MAX_ARGS*sizeof(int));
                // Go through all potential args
                for(int i = 1; i < command.argc; i++)
                {
                    // Check for flags
                    if(strncmp(command.argv[i], "-", 1) == 0)
                    {
                        if(strcmp(command.argv[i], "-r") == 0) recursive = true;
                        else if(strcmp(command.argv[i], "-v") == 0) verbose = true;
                        else if(strcmp(command.argv[i], "-b") == 0) mode = MODE_BINARY;
                        else if(strcmp(command.argv[i], "-s") == 0) stats = true;
                        else error = true;
                    }
                    else argPaths[j++] = i;
                }
                
                // Packed densely and as efficiently as possible, with minimal code redundancy
                if(!error && j != 0) 
                    if(strcmp(command.argv[0], "encrypt"))
                        for(j--; j >= 0; j--)
                            decryptFiles(command.argv[argPaths[j]], recursive, verbose, mode, stats);
                    else 
                        for(j--; j >= 0; j--)
                            encryptFiles(command.argv[argPaths[j]], recursive, verbose, mode, stats);
                   
                else printf("ms crypt: Please specify a file or directory, with optional flags beforehand to decrypt\n\t\
                             \rsubdirectories (recursive -r) and/or show files and directories found (verbose -v) e.g decrypt -r -v foo.txt\n"); 
            }
            else printf("ms: Must be logged in as user to decrypt\n");
            
            continue;
        }
        
        // End of built in shell commands
        //*********************************************************************************************************************************

        // Get the full pathname for the file
        command.name  = lookupPath(command.argv, pathv);

        if(command.name == NULL) 
        {
            fprintf(stderr, "Command %s not found\n", command.argv[0]);
            continue;
        }

        // Create a process to execute the command
        if((chPID = fork()) == 0) 
        {
            //  This is the child, that will execute the command requested
            execv(command.name, command.argv);
        }
        
        // Wait for the child to terminate
        finishedChPID = thisChPID = wait(&stat);
        
        if(finishedChPID == -1) 
		{
			printf("Error: child termination error\n");
			exit(1);
		}
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
        ERROR_PTR_PTR((result = (char *) malloc(strlen(argv[0])+1)));
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

        if(access(pName, X_OK | F_OK) != -1) 
        {

            ERROR_PTR_PTR((result = (char *) malloc(strlen(pName)+1)));
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
    while(argc < MAX_ARGS && (cmd->argv[argc++] = strsep(clPtr, WHITESPACE)) != NULL) ;
    
    // Modified code to allow escape characters ONLY FOR ' ' (spaces)
    // Any technical WHITESPACE escaped is assumed ' '
    int count = 0;
    int holdArgc = argc;
    while(count < argc - 1)
    {
        if((cmd->argv[count])[strlen(cmd->argv[count]) - 1] == '\\')
        {

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

    ERROR_PTR_NUM((pathEnvVar = (char *) getenv("PATH")));
    ERROR_PTR_NUM((thePath = (char *) malloc(strlen(pathEnvVar) + 1)));
    // Check malloc
    ERROR_PTR_NUM(thePath);
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

    return 0;

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
    ERROR_PTR_VOID(fgets(buffer, LINE_LEN, stdin));

    buffer[strlen(buffer)-1] = '\0';  // overwrite the line feed with null term
}

void encryptFiles(const char* file, bool recursive, bool verbose, bool mode, bool stats)
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
            ERROR_PTR_VOID((files  = malloc(sizeof(STACK_t*))));
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
            
            // Time evaluations
	        struct timeval StartTime, StopTime;
            long int bytes = 0;
            unsigned int microsecs;
            
            // Key and tag
            char* tag = loginGetUsername();
            const unsigned int key[2][2];
            
            // Definitely shouldn't be casting consts..
            if(!getKey((unsigned int (*)[2]) key))  
            {
                printf("Error in key generation. Abort.\n");
                clearStack(files, pathLocal);
                return;
            }
            
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
                    if(!pathLocal) free(curFile->keyValue);
                    free(curFile);
                    curFile = pop(files);
                    continue;
                }
                if((outputFile = fopen(newFile, "w+")) == NULL)
                {
                    printf("Could not open output file\n");
                    skippedFiles++;
                    fclose(inputFile);
                    if(!pathLocal) free(curFile->keyValue);
                    free(curFile);
                    curFile = pop(files);
                    continue;
                }
                if(!tagFile(inputFile, tag)) 
                {
                    // Fix the mess we made now and log it
                    printf("Could not tag file\n");
                    skippedFiles++;
                    fclose(inputFile);
                    fclose(outputFile);
                    remove(newFile);
                }
                else
                {
                    if(stats)
                    {
                        if(fseek(inputFile, 0L, SEEK_END) != 0)
						{
							printf("Encrypt: file seek error");
							continue;
						}
						int newBytes = ftell(inputFile);
						if(newBytes == -1) 
						{
							perror("Error reading file");
							continue;
						}
                        bytes += newBytes;
                    }
                    
                    gettimeofday(&StartTime, 0);
                    // We are go for encryption
                    if(!encrypt(inputFile, outputFile, key, mode)) {
                        gettimeofday(&StopTime, 0);
                        fclose(inputFile);
                        fclose(outputFile);
                        remove("temp.txt");
                        remove(newFile);
                        skippedFiles++;
                    }
                    else {
                        gettimeofday(&StopTime, 0);
                        fclose(inputFile);
                        fclose(outputFile);
                        remove(fileName);
                    }
                    
                    if(stats) {
                        
                        microsecs +=((StopTime.tv_sec - StartTime.tv_sec)*1000000);

                        if(StopTime.tv_usec > StartTime.tv_usec)
                            microsecs+=(StopTime.tv_usec - StartTime.tv_usec);
                        else
                            microsecs-=(StartTime.tv_usec - StopTime.tv_usec);
                    }
                }
                if(!pathLocal) free(curFile->keyValue);
                free(curFile);
                curFile = pop(files);
            }
            
            if(stats) 
            {
                printf("Stats:\n");
                printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microsecs, bytes, (float) bytes / microsecs * 1000000);
                printf("\tEncrypted %d of %d total files\n", encryptFiles - skippedFiles, totalFiles);
                printf("\tEncrypted %d of %d total non-encrypted files\n", encryptFiles - skippedFiles, encryptFiles);
                printf("\tSkipped %d of %d total non-encrypted files\n", skippedFiles, encryptFiles);
            }

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

void decryptFiles(const char* file, bool recursive, bool verbose, bool mode, bool stats)
{
    struct stat sb;
    bool pathLocal = false; // Did we just get path from file (single file) or dynamically from search?
    STACK_t* files = NULL;
    char checkDir[MAX_NAME] = "";
    
    // check to see if it is the home directory
    if(strcmp(file, "./") == 0) strcpy(checkDir, "./");
    else sprintf(checkDir, "./%s", file);
    
    // Make it so all inputs are based on base directory
    
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
            ERROR_PTR_VOID((files  = malloc(sizeof(STACK_t*))));
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
    // Confirm we have files to decrypt and proceed
    if(files != NULL  && files->head != NULL)
    {
        char inputBuffer[LINE_LEN];
        int totalFiles = files->size;
        printf("%d files will be decrypted, each file output as file.ext - .crpt\n\tProceed? [Yn]: ", totalFiles);
        readCommand(inputBuffer);
        if(inputBuffer[0] != '\0' && (inputBuffer[0] == 'y' || inputBuffer[0] == 'Y'))
        {
            // Decrypt the files!!! 
            int skippedFiles = 0;
            int decryptFiles = 0;
            
            // Time evaluations
	        struct timeval StartTime, StopTime;
            long int bytes = 0;
            unsigned int microsecs;
            
            // Key and tag
            const unsigned int key[2][2];
            char* tag = loginGetUsername();
            
            // Definitely shouldn't be casting consts..
            if(!getKey((unsigned int (*)[2]) key)) 
            {
                printf("Error in key generation. Abort.\n");
                clearStack(files, pathLocal);
                return;
            }
            
            // Start looping
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
                    // Not a valid file to decrypt
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
                    
                    
                    if(!pathLocal) free(curFile->keyValue);
                    free(curFile);
                    curFile = pop(files);
                    continue;
                }
                if((outputFile = fopen(newFile, "w+")) == NULL)
                {
                    printf("Could not open output file\n");
                    skippedFiles++;
                    fclose(inputFile);
                    
                    if(!pathLocal) free(curFile->keyValue);
                    free(curFile);
                    curFile = pop(files);
                    continue;
                }
                
                if(stats)
                {
                    fseek(inputFile, 0L, SEEK_END);
                    bytes += ftell(inputFile);
                }

                gettimeofday(&StartTime, 0);
                
                if(!decrypt(inputFile, outputFile, key, mode))
                {
                    gettimeofday(&StopTime, 0);
                    fclose(inputFile);
                    fclose(outputFile);
                    remove("temp.txt");
                    remove(newFile);
                    skippedFiles++;
                }
                else if(!checkTag(outputFile, tag)) 
                {
                    // Fix the freakin mess we made now, and log it
                    gettimeofday(&StopTime, 0);
                    printf("Couldn't verify tag\n");
                    skippedFiles++;
                    fclose(inputFile);
                    fclose(outputFile);
                    remove(newFile);
                }
                else
                {
                    // We are a-ok to remove the old file
                    gettimeofday(&StopTime, 0);
                    fclose(inputFile);
                    fclose(outputFile);
                    remove(fileName);
                }
                
                if(stats) {

                    microsecs +=((StopTime.tv_sec - StartTime.tv_sec)*1000000);

                    if(StopTime.tv_usec > StartTime.tv_usec)
                        microsecs+=(StopTime.tv_usec - StartTime.tv_usec);
                    else
                        microsecs-=(StartTime.tv_usec - StopTime.tv_usec);
                }

                if(!pathLocal) free(curFile->keyValue);
                free(curFile);
                curFile = pop(files);
            }
            if(stats) 
            {
                printf("Stats:\n");
                printf("\tTime: %u\n\tTotal Bytes: %ld\n\tBytes/sec: %2.2f\n", microsecs, bytes, (float) bytes / microsecs * 1000000);
                printf("\tDecrypted %d of %d total files\n", decryptFiles - skippedFiles, totalFiles);
                printf("\tDecrypted %d of %d total encrypted files\n", decryptFiles - skippedFiles, decryptFiles);
                printf("\tSkipped %d of %d total encrypted files\n", skippedFiles, decryptFiles);        
            }
        }
        else 
        {
            printf("decrypt: Decryption aborted\n");
            // Clear stack
            clearStack(files, pathLocal);
        }
    }
    else printf("decrypt: No files to decrypt\n");
}
