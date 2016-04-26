// Anthony Islas
// Date Written: 12 Dec 2014
// Last Revised:  6 Sept 2015
// DESCRIPTION:	This will be the base for logging into a shell that will
// not only exexcute standard UNIX commands, but act as a front end
// to enable encryption of files

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pwd.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>

#define _GNU_SOURCE   
#include <crypt.h>

#include "login.h"

// Global
static int current_user = -1, max_users = 0;
static char passphraseFile[PATH_MAX];
static char curName[MAX_NAME] = "default";

bool loginProtocol(char option)
{
  // Initialization
  bool enter_sp = false;
  users_t session_users[MAX_USERS];
	
	FILE* passphrase;
  
	if((passphrase = fopen(passphraseFile, "r+")) == NULL)
	{
		printf("File not found. Creating new file\n");
		if((passphrase = fopen(passphraseFile, "w+")) == NULL)
		{
			printf("Could not create file\n");
			return false;
		}
		if(option != 'n') 
		{
		  option = 'n';
		  printf("Defaulting to make new user...\n");
		}
	}
  
  // Defaults for users set first
  for(int iterator = MAX_USERS; iterator > 0; iterator--)
  {
    INIT_USER(session_users[iterator - 1]);
  }
  
	// Read in users from file
	PERROR_NUM_BOOL(fseek(passphrase, 0, SEEK_SET));
	max_users = fread((unsigned char*) session_users, sizeof(unsigned char), sizeof(users_t)*MAX_USERS, passphrase) / (sizeof(users_t));
	if(ferror(passphrase)) {
		printf("loginProtocol: error reading passphrase file\n");
		return false;
	}
	PERROR_NUM_BOOL(fclose(passphrase));
	// Makes sure users are only in memory
    
  switch(option)
  {
    case 'l':
      enter_sp = login(session_users);
      break;
    case 'n':
      enter_sp = newAccount(session_users);
      break;
    case 'd':
      enter_sp = deleteUser(session_users);
      break;
    default:
      enter_sp = false;
      printf("Oops. Looks like the option you requested wasn't found. Try again.\n");
      break;
  }

  return enter_sp;  
}

//************************************************************************
//
// Taken directly from minishell.c - emulate reading of CLI
// 
//************************************************************************

void readInput(char *buffer) 
{
    // original dangerous stdio call - can over-run buffer, see man gets
    // gets(buffer); 

  
    // suggested safer replacement call - can't go beyond length provided,
    // but we must strip off the line feed included in the buffer unlike gets
    // 
    ERROR_PTR_VOID(fgets(buffer, MAX_NAME, stdin));

    buffer[strlen(buffer) - 1] = '\0';  // overwrite the line feed with null term
}

//************************************************************************
//
// Modified readInput to allow getpass to be used instead of stdin
// 
//************************************************************************

void readPasswd(char *buffer, const char *salt, char *username) 
{
  char passwdPrompt[] = "Password [4-8 chars]: ";
  char* ps_buffer = getpass(passwdPrompt);
  int strLen = strlen(ps_buffer);
  if(strLen < MAX_PASSWD - 5 && strLen >= MIN_INPUT  && (strcmp(username, ps_buffer) != 0))
  {
    ps_buffer = crypt(ps_buffer, salt);
    if(ps_buffer == NULL)
    {
      buffer[0] = '\0';
      printf("Error in crypt\n");
    }
      
    buffer[strlen(ps_buffer)-1] = '\0';  // overwrite the line feed with null term
    strcpy(buffer, ps_buffer);
  }
  else if(strLen >= MAX_PASSWD - 5)
  {
    buffer[0] = '\0';
    printf("Error: Password too long\n"); 
  }
  else if(strLen < MIN_INPUT)
  {
    buffer[0] = '\0';
    printf("Error: Password too short\n"); 
  }
  else 
  {
    buffer[0] = '\0';
    printf("Error: Cannot assign username as password\n");
  }
}

//************************************************************************
//
// Protocol for login
// 
//************************************************************************

bool login(users_t* cur_list)
{
  // Initialize
  char cl_buffer[MAX_PASSWD];
  
  // Prompt
  printf("Enter username: ");
  readInput(cl_buffer);
  
  // Search first for username
  for(int iterator = MAX_USERS; iterator > 0; iterator--)
  {
    // If successful ask for password
    if(strcmp(cur_list[MAX_USERS - iterator].user_name,cl_buffer) == 0)
    {
      // prompt 

      readPasswd(cl_buffer, cur_list[MAX_USERS - iterator].user_passwd, cur_list[MAX_USERS - iterator].user_name);
      // Check password to user account
      if(strcmp(cur_list[MAX_USERS - iterator].user_passwd,cl_buffer) == 0) 
      {
        current_user = MAX_USERS - iterator;
        printf("Logged in as: %s", cur_list[MAX_USERS - iterator].user_name);
        strcpy(curName, cur_list[MAX_USERS - iterator].user_name);
        current_user == 0 ? printf(" [ADMIN]\n") : printf("\n");
        return true;
      }
      else
      {
        // Wrong password
        int err_login = 0;
        while(err_login < TRIES)
        {
          // Loop allows for a retry 3 times
          printf("Sorry, try again.\n");
          readPasswd(cl_buffer, cur_list[MAX_USERS - iterator].user_passwd, cur_list[MAX_USERS - iterator].user_name);
          // Auto exit if correct
          if(strcmp(cur_list[MAX_USERS - iterator].user_passwd,cl_buffer) == 0)
          {
            current_user = MAX_USERS - iterator;
            printf("Logged in as: %s", cur_list[MAX_USERS - iterator].user_name);
            strcpy(curName, cur_list[MAX_USERS - iterator].user_name);
            current_user == 0 ? printf(" [ADMIN]\n") : printf("\n");
            return true;
          }
          err_login++;
        }
        // If not exited by now, return false - failed login
        printf("ERROR: Login fail.\n");
        return false;
      }
    }
    // Continue through list
  } // End search
  // Search unsuccessful
  printf("ERROR: Oops. Username not found. Login fail.\n");
  return false;
}

//************************************************************************
//
// Protocol for creating a new account (currently just for session
//
//************************************************************************

bool newAccount(users_t* cur_list)
{
  //
  // Create new session account
  //
  
  // Initialize
  char cl_buffer[MAX_NAME];
  int temp_user = current_user;
	
	FILE* passphrase;
	if((passphrase = fopen(passphraseFile, "r+")) == NULL)
	{
		printf("File not found.\n");
		return false;
	}
	// Makes sure users are only in memory
  
  // Search first for username spot available
  for(int iterator = MAX_USERS; iterator > 0; iterator--)
  {
    // If successful create account
    if(strcmp(cur_list[MAX_USERS - iterator].user_name, DEF_CHAR) == 0)
    {
      current_user = MAX_USERS - iterator;
      break;
    }
  } // search end
  // If successful, current_user should have changed
  if(temp_user == current_user)  // No available spot
  {
    printf("Error: No spot available to create account. Please contact:\n\tAdmin: USER[0] to get account\n");
    // TODO Implement admin priveleges and deletion of accounts
    PERROR_NUM_BOOL(fclose(passphrase));
    return false;
  }
  
  // ELSE make account
  printf("Creating a new Account for USER[%d]:\n", current_user);
  // Prompt
  
  printf("Username[%d-%d chars]: ", MIN_INPUT, MAX_NAME - 1);
  readInput(cl_buffer);
  printf("\tUsername: %s\n", cl_buffer);
  int nameSize = strlen(cl_buffer);
  if((nameSize < MAX_NAME - 1) && nameSize >= MIN_INPUT)
  {
    // Search if username already taken
    for(int iterator = MAX_USERS; iterator > 0; iterator--)
    {
      // If successful create account
      if(strcmp(cur_list[MAX_USERS - iterator].user_name, cl_buffer) == 0)
      {
        printf("Error: Username not available\n");
        current_user = temp_user;
        PERROR_NUM_BOOL(fclose(passphrase));
        return false;
      }
    } // search end
    strcpy(cur_list[current_user].user_name, cl_buffer);
    strcpy(curName, cur_list[current_user].user_name);
  }
  else if(nameSize >= MAX_NAME)
  {
    printf("Error: Username too long\n");
    PERROR_NUM_BOOL(fclose(passphrase));
	current_user = temp_user;
    return false;
  }
  else if(nameSize < MIN_INPUT)
  {
    printf("Error: Username too short\n");
    PERROR_NUM_BOOL(fclose(passphrase));
	current_user = temp_user;
    return false;
  }
  
  // Set password
  // Seed rand with user's first letter and its length
  // then assign salt
  srand(((unsigned int) cur_list[current_user].user_name[0] + strlen(cur_list[current_user].user_name)));
  const char salt[3] = {rand() % 26 + ASCII_A, rand() % 26 + ASCII_A, '\0'};
  readPasswd(cl_buffer, salt, cur_list[current_user].user_name);
  
  if(cl_buffer[0] != '\0')
  {
    strcpy(cur_list[current_user].user_passwd, cl_buffer);
		cur_list[current_user].user_id = current_user;
		
		// Update PASSPHRASE file
		PERROR_NUM_BOOL(fseek(passphrase, 0, SEEK_SET));
		fwrite((unsigned char*) cur_list, sizeof(unsigned char), sizeof(users_t)*(current_user + 1), passphrase);
    PERROR_NUM_BOOL(fclose(passphrase));
    printf("Logged in as: %s", cur_list[current_user].user_name);
    current_user == 0 ? printf(" [ADMIN]\n") : printf("\n");
    // Makes sure users are only in memory
		
		return true;
  }
  else 
  {
    printf("Error: Could not create user. Password error in length or same as username\n");
    INIT_USER(cur_list[current_user]);
    current_user = temp_user;
    PERROR_NUM_BOOL(fclose(passphrase));
		return false;
  }
}


//************************************************************************
//
// Protocol for delete user
// 
//************************************************************************
// Note: Must be admin USER[0] to use this option!
bool deleteUser(users_t* cur_list)
{ 
  char cl_buffer[MAX_NAME];
  if(current_user >= 1)
  {
    printf("Delete account? [Yn]: ");
    readInput(cl_buffer);
    if(cl_buffer[0] == 'Y' || cl_buffer[0] == 'y')
    {
      INIT_USER(cur_list[current_user]); 

      // Update passphrase file
      FILE* passphrase;
      if((passphrase = fopen(passphraseFile, "r+")) == NULL)
      {
        printf("File not found.\n");
        return false;
      }
      PERROR_NUM_BOOL(fseek(passphrase, 0, SEEK_SET));
      fwrite((unsigned char*) cur_list, sizeof(unsigned char), sizeof(users_t)*max_users, passphrase);
      PERROR_NUM_BOOL(fclose(passphrase));

      printf("Deleted USER[%d] account.\n", current_user);
      current_user = -1;
      return true;
    }
    return false;
  }
  else if(current_user == 0)
  {
    // Admin user
    printf("Enter username: ");
    readInput(cl_buffer);

    // Stolen from login protocol
    // Search first for username
    for(int iterator = MAX_USERS; iterator > 0; iterator--)
    {
      // If successful verify deletion and write out
      if(strcmp(cur_list[MAX_USERS - iterator].user_name,cl_buffer) == 0)
      {
        printf("Delete user ""%s""? [Yn]: ", cur_list[MAX_USERS - iterator].user_name);
        readInput(cl_buffer);
        // Assume everything else is N
        if(cl_buffer[0] == 'Y' || cl_buffer[0] == 'y')
        {
          INIT_USER(cur_list[MAX_USERS - iterator]); 

          // Update passphrase file
          FILE* passphrase;
          if((passphrase = fopen(passphraseFile, "r+")) == NULL)
          {
            printf("File not found.\n");
            return false;
          }
          PERROR_NUM_BOOL(fseek(passphrase, 0, SEEK_SET));
          fwrite((unsigned char*) cur_list, sizeof(unsigned char), sizeof(users_t)*max_users, passphrase);
          PERROR_NUM_BOOL(fclose(passphrase));

          // If admin deleted self
          if((MAX_USERS - iterator) == 0)
          {
            current_user = -1;
            printf("Logged admin out\n");
          }

          printf("Deleted USER[%d] account.\n", MAX_USERS - iterator);
          return true;
        }
        return false;
      }
      // Continue through list
    } // End search
    // Search unsuccessful
    printf("Error: Oops. Username not found. Login fail.\n");
    return false;
  }
  return false;
}


//************************************************************************
//
// Accessors
// 
//************************************************************************
char* loginGetUsername(void)
{
  return curName;
}

//************************************************************************
//
// Initilize the module, fix bug where passphrase file was dependent on 
// working directory of program
//
//************************************************************************
bool LoginModuleInit(void)
{
  uid_t uid = geteuid();
  errno = 0;
  
  struct passwd *pw = getpwuid(uid);
  
  // Check if errno has changed
  PERROR_NUM_BOOL((errno != 0 ? -1 : 0));
  if (pw)
  {
    sprintf(passphraseFile, "/home/%s/%s", pw->pw_name, PASSPHRASE);
    return true;
  }
  else return false;
}

int loginGetCurUser(void)
{
  return current_user; 
}

void loginSetCurUser(int newCurUser)
{
  current_user = newCurUser; 
}