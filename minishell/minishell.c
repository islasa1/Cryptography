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
#include	<pthread.h>
#include	<semaphore.h>
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
bool runningThreads[MAX_THREADS];
pthread_mutex_t bytesMutex;
pthread_mutex_t microsecsMutex;
pthread_mutex_t skippedMutex;
pthread_mutex_t encryptMutex;
pthread_mutex_t decryptMutex;

sem_t threadSemaphore;


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

    memset(runningThreads, FALSE, sizeof(bool)*MAX_THREADS);
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
    
    pthread_mutexattr_t mutexType;
	pthread_mutexattr_settype(&mutexType, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&bytesMutex, &mutexType);
	pthread_mutex_init(&microsecsMutex, &mutexType);
	pthread_mutex_init(&skippedMutex, &mutexType);
    pthread_mutex_init(&encryptMutex, &mutexType);
    pthread_mutex_init(&decryptMutex, &mutexType);
	sem_init(&threadSemaphore, 0, MAX_THREADS);

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
            else if((strcmp(command.argv[1], "-d") == 0) && loggedIn)
            {
                if(!loginProtocol('d')) printf("Could not delete user or quit.\n");
                if(loginGetCurUser() == -1)
                {
                    loggedIn = false;
                    sprintf(promptString, " > ");
                }
            }
            else
            {
                printf("login: Please specify if logging in as existing user -l, or new user -n. Must be logged in to delete user\n");    
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
                bool recursive = false, verbose = false, mode = MODE_TEXT, stats = false, threading = false;

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
                        else if(strcmp(command.argv[i], "-t") == 0) threading = true;
                        else error = true;
                    }
                    else argPaths[j++] = i;
                }
                
                // Packed densely and as efficiently as possible, with minimal code redundancy
                if(!error && j != 0) 
                    if(strcmp(command.argv[0], "encrypt"))
                        for(j--; j >= 0; j--)
                            decryptFiles(command.argv[argPaths[j]], recursive, verbose, mode, stats, threading);
                    else 
                        for(j--; j >= 0; j--)
                            encryptFiles(command.argv[argPaths[j]], recursive, verbose, mode, stats, threading);
                   
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
        free(command.name);
        
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

void encryptFiles(const char* file, bool recursive, bool verbose, bool mode, bool stats, bool threading)
{
    struct stat sb;
    bool pathLocal = false; // Did we just get path from file (single file) or dynamically from search?
    STACK_t* files = NULL;
    char checkDir[NAME_MAX] = "";
    int errorCode = 0;
    
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
        printf("encrypt: Could not encrypt. Argument to encrypt is neither a file nor a directory\n");
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
			
			// Threads for encrypting
			pthread_t threads[MAX_THREADS];
            
            // Time evaluations
	        //struct timeval StartTime, StopTime;
            long int bytes = 0;
            unsigned int microsecsAvg = 0, microsecsRaw = 0;
            
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
            
            // If not threading take away resources from semaphore
            if(!threading) for(int i = 0; i < MAX_THREADS - 1; i++) if(sem_wait(&threadSemaphore) == -1)
            {
                perror("encrypt: sem_wait() error: ");
                clearStack(files, pathLocal);
                return;
            }
            
            struct timeval startTime, stopTime;
            itemS_t* curFile = pop(files);
			
            // ************************************************************************************
            // MAIN BLOCK OF CODE TO TEST IOPS
            if(gettimeofday(&startTime, 0) == -1) perror("encrypt: gettimofday() error: ");
            
            while(curFile != NULL)
            {
                struct crypt_thread_args_t *args = (struct crypt_thread_args_t *)malloc(sizeof(struct crypt_thread_args_t));
                if(args == NULL) {
                    printf("encrypt: malloc() error: Skipping file!\n");
                    pthread_mutex_lock(&skippedMutex);
                    skippedFiles++;
                    pthread_mutex_unlock(&skippedMutex);
                    if(!pathLocal) free(curFile->keyValue);
                }
                else {

                    // Initialize argument struct with argument values
                    args->fileName = CHAR_PTR curFile->keyValue;
                    args->tag = tag;
                    args->key = &key;
                    args->mode = mode;
                    args->pathLocal = pathLocal;
                    args->stats = stats;
                    args->microsecs = &microsecsAvg; args->bytes = &bytes;
                    args->skippedFiles = &skippedFiles;
                    args->encryptFiles = &encryptFiles;

                    if(sem_wait(&threadSemaphore) == -1) {
                        perror("encrypt: sem_wait() error: ");
                        free(args);
                        if(!pathLocal) free(curFile->keyValue);
                        pthread_mutex_lock(&skippedMutex);
                        skippedFiles++;
                        pthread_mutex_unlock(&skippedMutex);
                    }
                    else {   
                        uint8_t i;
                        for(i = 0; runningThreads[i] == TRUE && i < (threading ? MAX_THREADS : 0); i++);

                        args->threadNum = i;
                        if((errorCode = pthread_create(&threads[i], NULL, threadedEncrypt, (void *)args)) > 0) {
                            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
                            free(args);
                            if(!pathLocal) free(curFile->keyValue);
                            pthread_mutex_lock(&skippedMutex);
                            skippedFiles++;
                            pthread_mutex_unlock(&skippedMutex);
                        }
                        else runningThreads[i] = TRUE;
                    }
                }
                
                
                // Some neat stats while we wait..
                if(threading && stats) {
                    printf("Running Threads: ");
                    for(int j = 0; j < MAX_THREADS; j++) printf("[%d]", runningThreads[j]);
                    printf("\r");
                }
                
                free(curFile);
                curFile = pop(files);
            }
            
            free(files);

            // Pseudo pthread_join()
            for(int i = 0; i < MAX_THREADS; i++) while(runningThreads[i]);
                
            // Real pthread_join()
            if(threading) 
                for (int i = 0; i < MAX_THREADS; i++) 
                    if (pthread_join (threads[i], NULL) < 0)
                        perror ("pthread_join");
                
            // If not threading give resources back to semaphore
            if(!threading) for(int i = 0; i < MAX_THREADS - 1; i++) if(sem_post(&threadSemaphore) == -1)
            {
                perror("encrypt: sem_wait() error: ");
                return;
            }
                
            if(gettimeofday(&stopTime, 0) == -1) perror("encrypt: gettimofday() error: ");

            // ************************************************************************************
            
            // Do some fancy shmancy math~
            microsecsRaw += ((stopTime.tv_sec - startTime.tv_sec)*1000000);

            if(stopTime.tv_usec > startTime.tv_usec)
                microsecsRaw += (stopTime.tv_usec - startTime.tv_usec);
            else
                microsecsRaw -= (startTime.tv_usec - stopTime.tv_usec);
            
            
            if(stats) 
            {
                printf("Stats:                                                                \n");
                printf("\tCummulative Time: %u\n\tRaw Time: %u\n", microsecsAvg,microsecsRaw);
                printf("\tTotal Bytes: %ld\n\tAvg Bytes/sec: %2.2f\n", bytes, (float) bytes / microsecsAvg * 1000000);
                printf("\tRaw Bytes/sec: %2.2f\n", (float) bytes / microsecsRaw * 1000000);
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

void decryptFiles(const char* file, bool recursive, bool verbose, bool mode, bool stats, bool threading)
{
    struct stat sb;
    bool pathLocal = false; // Did we just get path from file (single file) or dynamically from search?
    STACK_t* files = NULL;
    char checkDir[NAME_MAX] = "";
    int errorCode = 0;
    
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
			
			// Threads for decrypting
			pthread_t threads[MAX_THREADS];
            
            // Time evaluations
	        //struct timeval StartTime, StopTime;
            long int bytes = 0;
            unsigned int microsecsAvg = 0, microsecsRaw = 0;
            
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
        
            // If not threading take away resources from semaphore
            if(!threading) for(int i = 0; i < MAX_THREADS - 1; i++) if(sem_wait(&threadSemaphore) == -1)
            {
                perror("encrypt: sem_wait() error: ");
                clearStack(files, pathLocal);
                return;
            }
    
            struct timeval startTime, stopTime;
            itemS_t* curFile = pop(files);
			
            // ************************************************************************************
            // MAIN BLOCK OF CODE TO TEST IOPS
            if(gettimeofday(&startTime, 0) == -1) perror("decrypt: gettimofday() error: ");

            while(curFile != NULL)
            {
                struct crypt_thread_args_t *args = (struct crypt_thread_args_t *)malloc(sizeof(struct crypt_thread_args_t));
                if(args == NULL) {
                    printf("decrypt: malloc() error: Skipping file!\n");
                    pthread_mutex_lock(&skippedMutex);
                    skippedFiles++;
                    pthread_mutex_unlock(&skippedMutex);
                    if(!pathLocal) free(curFile->keyValue);
                }
                else {

                    // Initialize argument struct with argument values
                    args->fileName = CHAR_PTR curFile->keyValue;
                    args->tag = tag;
                    args->key = &key;
                    args->mode = mode;
                    args->pathLocal = pathLocal;
                    args->stats = stats;
                    args->microsecs = &microsecsAvg; args->bytes = &bytes;
                    args->skippedFiles = &skippedFiles;
                    args->decryptFiles = &decryptFiles;

                    if(sem_wait(&threadSemaphore) == -1) {
                        perror("decrypt: sem_wait() error: ");
                        free(args);
                        if(!pathLocal) free(curFile->keyValue);
                        pthread_mutex_lock(&skippedMutex);
                        skippedFiles++;
                        pthread_mutex_unlock(&skippedMutex);
                    }
                    else {   
                        uint8_t i;
                        for(i = 0; runningThreads[i] == TRUE && i < (threading ? MAX_THREADS : 0); i++);

                        args->threadNum = i;
                        if((errorCode = pthread_create(&threads[i], NULL, threadedDecrypt, (void *)args)) > 0) {
                            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
                            free(args);
                            if(!pathLocal) free(curFile->keyValue);
                            pthread_mutex_lock(&skippedMutex);
                            skippedFiles++;
                            pthread_mutex_unlock(&skippedMutex);
                        }
                        else runningThreads[i] = TRUE;
                    }
                }
                
                
                // Some neat stats while we wait..
                if(threading && stats) {
                    printf("Running Threads: ");
                    for(int j = 0; j < MAX_THREADS; j++) printf("[%d]", runningThreads[j]);
                    printf("\r");
                }
                
                free(curFile);
                curFile = pop(files);
            }
            
            free(files);

            // Pseudo pthread_join()
            for(int i = 0; i < MAX_THREADS; i++) while(runningThreads[i]);
                
            // Real pthread_join()
            if(threading)
                for (int i = 0; i < MAX_THREADS; i++) 
                    if (pthread_join (threads[i], NULL) < 0) 
                        perror ("pthread_join");
                
            // If not threading give resources back to semaphore
            if(!threading) for(int i = 0; i < MAX_THREADS - 1; i++) if(sem_post(&threadSemaphore) == -1)
            {
                perror("decrypt: sem_wait() error: ");
                return;
            }
                
            if(gettimeofday(&stopTime, 0) == -1) perror("decrypt: gettimofday() error: ");
            
            // ************************************************************************************
            
            // Do some fancy shmancy math~
            microsecsRaw += ((stopTime.tv_sec - startTime.tv_sec)*1000000);

            if(stopTime.tv_usec > startTime.tv_usec)
                microsecsRaw += (stopTime.tv_usec - startTime.tv_usec);
            else
                microsecsRaw -= (startTime.tv_usec - stopTime.tv_usec);
            
            
            if(stats) 
            {
                printf("Stats:                                                                \n");
                printf("\tCummulative Time: %u\n\tRaw Time: %u\n", microsecsAvg,microsecsRaw);
                printf("\tTotal Bytes: %ld\n\tAvg Bytes/sec: %2.2f\n", bytes, (float) bytes / microsecsAvg * 1000000);
                printf("\tRaw Bytes/sec: %2.2f\n", (float) bytes / microsecsRaw * 1000000);
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

void * threadedEncrypt(void *args)
{
    struct crypt_thread_args_t argsStruct = *(struct crypt_thread_args_t *)args;
	free(args);
    int errorCode = 0;
    
    char newFile[NAME_MAX] = "";
    char* crptExt = strrchr(argsStruct.fileName, '.');
    
    if(crptExt != NULL && (strcmp(crptExt, ".crpt") != 0))
    {
        // Valid file to encrypt
        sprintf(newFile, "%s.crpt", argsStruct.fileName);
        if((errorCode = pthread_mutex_lock(&encryptMutex)) > 0)
        {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
            
            return NULL;
        }  
        (*(argsStruct.encryptFiles))++;
        if((errorCode = pthread_mutex_unlock(&encryptMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
            
            return NULL;
        }  
    }
    else
    {
        if(!argsStruct.pathLocal) free(argsStruct.fileName);
        runningThreads[argsStruct.threadNum] = FALSE;
	    if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error: ");
        
        return NULL;
    }
    
    FILE *inputFile, *outputFile;
    if((inputFile = fopen(argsStruct.fileName, "r+")) == NULL)
    {
        printf("File not found\n");
        if((errorCode = pthread_mutex_lock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
            
            return NULL;
        }
        (*(argsStruct.skippedFiles))++;
        if((errorCode = pthread_mutex_unlock(&skippedMutex))  > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
            
            return NULL;
        }
        if(!argsStruct.pathLocal) free(argsStruct.fileName);
        runningThreads[argsStruct.threadNum] = FALSE;
	    if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error: ");
        
        return NULL;
    }
    if((outputFile = fopen(newFile, "w+")) == NULL)
    {
        printf("Could not open output file\n");
        if((errorCode = pthread_mutex_lock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
            
            return NULL;
        }
        (*(argsStruct.skippedFiles))++;
        if((errorCode = pthread_mutex_unlock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
            
            return NULL;
        }
        if(fclose(inputFile) == EOF) perror("fclose error: ");
        if(!argsStruct.pathLocal) free(argsStruct.fileName);
        runningThreads[argsStruct.threadNum] = FALSE;
	    if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post(0 error: ");
        
        return NULL;
    }
    if(!tagFile(inputFile, argsStruct.tag)) 
    {
        // Fix the mess we made now and log it
        printf("Could not tag file\n");
        if((errorCode = pthread_mutex_lock(&skippedMutex) > 0)) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
            
            return NULL;
        }
        (*(argsStruct.skippedFiles))++;
        if((errorCode = pthread_mutex_unlock(&skippedMutex) > 0)) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
            
            return NULL;
        }
        if(fclose(inputFile)  == EOF) perror("fclose error: ");
        if(fclose(outputFile) == EOF) perror("fclose error: ");
        if(remove(newFile) == EOF) perror("remove error: ");
    }
    else
    {
        struct timeval startTime, stopTime;

        if(gettimeofday(&startTime, 0) == -1) {
            perror("threadedEncrypt: gettimeofday() error: "); 
        }
        // We are go for encryption
        if(!encrypt(inputFile, outputFile, (*argsStruct.key), argsStruct.mode)) {
            if(gettimeofday(&stopTime, 0) == -1) perror("threadedEncrypt: gettimeofday() error: ");
            if(fclose(inputFile) == EOF) perror("fclose error: ");
            if(fclose(outputFile)  == EOF) perror("fclose error: ");
            if(remove(newFile)  == EOF) perror("fclose error: ");
            if((errorCode = pthread_mutex_lock(&skippedMutex)) > 0) {
                printf("Error at %d, %s", __LINE__, strerror(errorCode) );
                if(!argsStruct.pathLocal) free(argsStruct.fileName);
                runningThreads[argsStruct.threadNum] = FALSE;
                if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
                
                return NULL;
            }
            (*(argsStruct.skippedFiles))++;
            if((errorCode = pthread_mutex_unlock(&skippedMutex) > 0)) {
                printf("Error at %d, %s", __LINE__, strerror(errorCode) );
                if(!argsStruct.pathLocal) free(argsStruct.fileName);
                runningThreads[argsStruct.threadNum] = FALSE;
                if(sem_post(&threadSemaphore) == -1)  perror("threadedEncrypt: sem_post() error:");
                
                return NULL;
            }
        }
        else {
            // Encryption Worked, clean up your mess!
            if(argsStruct.stats)
            {   
                if(fseek(inputFile, 0L, SEEK_END) == -1) {
                    perror("threadedEncrypt: fseek() error: ");
                    if(!argsStruct.pathLocal) free(argsStruct.fileName);
                    runningThreads[argsStruct.threadNum] = FALSE;
                    if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
                    
                    return NULL;
                }
                long int bytesToAdd = ftell(inputFile);
                if(bytesToAdd == -1) {
                    perror("threadedEncrypt: ftell() error: ");
                    bytesToAdd = 0;
                }
                
                if((errorCode = pthread_mutex_lock(&bytesMutex)) > 0) {
                    printf("Error at %d, %s", __LINE__, strerror(errorCode) );
                    if(!argsStruct.pathLocal) free(argsStruct.fileName);
                    runningThreads[argsStruct.threadNum] = FALSE;
                    if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
                    
                    return NULL;
                }
                *(argsStruct.bytes) += bytesToAdd;
                if((errorCode = pthread_mutex_unlock(&bytesMutex)) > 0) {
                    printf("Error at %d, %s", __LINE__, strerror(errorCode) );
                    if(!argsStruct.pathLocal) free(argsStruct.fileName);
                    runningThreads[argsStruct.threadNum] = FALSE;
                    if(sem_post(&threadSemaphore) == -1) perror("threadedEncrypt: sem_post() error:");
                    
                    return NULL;
                }
            }
            if(gettimeofday(&stopTime, 0) == -1) perror("threadedEncrypt: gettimeofday() error: "); 
            
            if(fclose(inputFile) == EOF) perror("fclose error:");
            if(fclose(outputFile)  == EOF) perror("fclose error:");
            if(remove(argsStruct.fileName)  == EOF) perror("fclose error:");
        }

        if(argsStruct.stats) {
            pthread_mutex_lock(&microsecsMutex);
            *(argsStruct.microsecs) += ((stopTime.tv_sec - startTime.tv_sec)*1000000);

            if(stopTime.tv_usec > startTime.tv_usec)
                *(argsStruct.microsecs) += (stopTime.tv_usec - startTime.tv_usec);
            else
                *(argsStruct.microsecs) -= (startTime.tv_usec - stopTime.tv_usec);
            pthread_mutex_unlock(&microsecsMutex);
        }
    }
    if(!argsStruct.pathLocal) free(argsStruct.fileName);
    
	runningThreads[argsStruct.threadNum] = FALSE;
    
	sem_post(&threadSemaphore);
	return NULL;
}

void * threadedDecrypt(void *args)
{
	struct crypt_thread_args_t argsStruct = *(struct crypt_thread_args_t *)args;
	free(args);
    
    int errorCode = 0;
    
    char newFile[NAME_MAX] = "";
    char* crptExt = strrchr(argsStruct.fileName, '.');

    // Yay, pointer math
    if(crptExt != NULL && (strcmp(crptExt, ".crpt") == 0))
    {
        // Valid file to decrypt
        // +1 for '\0'
        strncpy(newFile, argsStruct.fileName, (abs(crptExt - argsStruct.fileName)));
        newFile[abs(crptExt - argsStruct.fileName)] = '\0';
        
        if((errorCode = pthread_mutex_lock(&decryptMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
            
            return NULL;
        }
        (*(argsStruct.decryptFiles))++;
        if((errorCode = pthread_mutex_unlock(&decryptMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
            
            return NULL;
        }
    }
    else
    {
        // Not a valid file to decrypt
        if(!argsStruct.pathLocal) free(argsStruct.fileName);
        runningThreads[argsStruct.threadNum] = FALSE;
	    sem_post(&threadSemaphore);
        return NULL;
    }

    FILE *inputFile, *outputFile;
    
    if((inputFile = fopen(argsStruct.fileName, "r+")) == NULL)
    {
        printf("File not found\n");
        if((errorCode = pthread_mutex_lock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
            
            return NULL;
        }
        (*(argsStruct.skippedFiles))++;
        if((errorCode = pthread_mutex_unlock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
            
            return NULL;
        }

        if(!argsStruct.pathLocal) free(argsStruct.fileName);
        runningThreads[argsStruct.threadNum] = FALSE;
	    sem_post(&threadSemaphore);
        return NULL;
    }
    if((outputFile = fopen(newFile, "w+")) == NULL)
    {
        printf("Could not open output file\n");
        if((errorCode = pthread_mutex_lock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
            
            return NULL;
        }
        (*(argsStruct.skippedFiles))++;
        if((errorCode = pthread_mutex_unlock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
           
            return NULL;
        }
        if(fclose(inputFile) == EOF)  perror("fclose error:");
        
        if(!argsStruct.pathLocal) free(argsStruct.fileName);
        runningThreads[argsStruct.threadNum] = FALSE;
	    sem_post(&threadSemaphore);
        return NULL;
    }
	
    
	struct timeval startTime, stopTime;
	gettimeofday(&startTime, 0);

	if(!decrypt(inputFile, outputFile, (*argsStruct.key), argsStruct.mode))
	{
        printf("Decryption Failure!: "); printf("%s\n", argsStruct.fileName);
		gettimeofday(&stopTime, 0);
		if(fclose(inputFile) == EOF) perror("fclose error: ");
		if(fclose(outputFile) == EOF) perror("fclose error: ");
		char tempFileName[NAME_MAX];
		sprintf(tempFileName, "temp%d.txt", fileno(inputFile));
		if(remove(tempFileName) == EOF) perror("remove error: ");
		if(remove(newFile) == EOF) perror("remove error: ");
        
		if((errorCode = pthread_mutex_lock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
           
            return NULL;
        }
        (*(argsStruct.skippedFiles))++;
        if((errorCode = pthread_mutex_unlock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
            
            return NULL;
        }
	}
	else if(!checkTag(outputFile, argsStruct.tag)) 
	{
		// Fix the freakin mess we made now, and log it
		if(gettimeofday(&stopTime, 0) == -1) perror("gettimeofday error: :");
		printf("Couldn't verify tag: "); printf("%s\n", newFile);
        
		if((errorCode = pthread_mutex_lock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
            
            return NULL;
        }
        (*(argsStruct.skippedFiles))++;
        if((errorCode = pthread_mutex_unlock(&skippedMutex)) > 0) {
            printf("Error at %d, %s", __LINE__, strerror(errorCode) );
            if(!argsStruct.pathLocal) free(argsStruct.fileName);
            runningThreads[argsStruct.threadNum] = FALSE;
            if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
            
            return NULL;
        }
        
		if(fclose(inputFile) == EOF) perror("fclose error: ");
		if(fclose(outputFile) == EOF) perror("fclose error: ");
		if(remove(newFile) == EOF) perror("remove error: ");
	}
	else
	{
		// We are a-ok to remove the old file
        if(gettimeofday(&stopTime, 0) == -1) perror("gettimeofday error: ");
        if(argsStruct.stats)
        {
            long int bytesToAdd = ftell(inputFile);
                
            if(fseek(inputFile, 0L, SEEK_END) == -1) {
                perror("threadedDecrypt: fseek() error: ");
                if(!argsStruct.pathLocal) free(argsStruct.fileName);
                runningThreads[argsStruct.threadNum] = FALSE;
                if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
                
                return NULL;
            }
            if(bytesToAdd == -1) {
                perror("threadedDecrypt: ftell() error: ");
                bytesToAdd = 0;
            }

            if((errorCode = pthread_mutex_lock(&bytesMutex)) > 0) {
                printf("Error at %d, %s", __LINE__, strerror(errorCode) );
                if(!argsStruct.pathLocal) free(argsStruct.fileName);
                runningThreads[argsStruct.threadNum] = FALSE;
                if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
                
                return NULL;
            }
            *(argsStruct.bytes) += bytesToAdd;
            if((errorCode = pthread_mutex_unlock(&bytesMutex)) > 0) {
                printf("Error at %d, %s", __LINE__, strerror(errorCode) );
                if(!argsStruct.pathLocal) free(argsStruct.fileName);
                runningThreads[argsStruct.threadNum] = FALSE;
                if(sem_post(&threadSemaphore) == -1) perror("threadedDecrypt: sem_post() error:");
                
                return NULL;
            }
        }
		if(fclose(inputFile)  == EOF) perror("fclose error: ");
		if(fclose(outputFile) == EOF) perror("fclose error: ");
		if(remove(argsStruct.fileName) == EOF) perror("remove error: ");
	}

	if(argsStruct.stats) {

		*(argsStruct.microsecs) += ((stopTime.tv_sec - startTime.tv_sec)*1000000);

		if(stopTime.tv_usec > startTime.tv_usec)
			*(argsStruct.microsecs) += (stopTime.tv_usec - startTime.tv_usec);
		else
			*(argsStruct.microsecs) -= (startTime.tv_usec - stopTime.tv_usec);
	}
    
    if(!argsStruct.pathLocal) free(argsStruct.fileName);
	runningThreads[argsStruct.threadNum] = FALSE;
	sem_post(&threadSemaphore);
    
	return NULL;
}
