// Anthony Islas
// Date Written: 12 Dec 2014
// Last Revised:  6 Sept 2015
// DESCRIPTION:	This will be the base for logging into a shell that will
// not only exexcute standard UNIX commands, but act as a front end
// to enable encryption of files

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#define _GNU_SOURCE   
#include <crypt.h>

#include "login.h"

// Global
static int current_user;

bool loginProtocol(char option)
{
  // Initialization
  bool enter_sp = false;
  users_t session_users[MAX_USERS];
  time_t t = time(NULL);
  current_user =-1;
	
	FILE* passphrase;
	if((passphrase = fopen(PASSPHRASE, "r+")) == NULL)
	{
		printf("File not found. Creating new file\n");
		if((passphrase = fopen(PASSPHRASE, "w+")) == NULL)
		{
			printf("Could not create file\n");
			exit(-1);
		}
	}
  
  // Defaults for users set first
  for(int iterator = MAX_USERS; iterator > 0; iterator--)
  {
    INIT_USER(session_users[iterator - 1]);
  }
  
	// Read in users from file
	fseek(passphrase, 0, SEEK_SET);
	int totalBytes = fread((unsigned char*) session_users, sizeof(unsigned char), sizeof(users_t)*MAX_USERS, passphrase);
	fclose(passphrase);
	// Makes sure users are only in memory
    
  switch(option)
  {
    case 'l':
      enter_sp = login(session_users);
      break;
    case 'n':
      enter_sp = newAccount(session_users);
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

void readCommand(char *buffer) 
{
    // original dangerous stdio call - can over-run buffer, see man gets
    // gets(buffer); 

  
    // suggested safer replacement call - can't go beyond length provided,
    // but we must strip off the line feed included in the buffer unlike gets
    // 
    fgets(buffer, LINE_LEN, stdin);

    buffer[strlen(buffer)-1] = '\0';  // overwrite the line feed with null term

    //printf("readCommand buffer=%s\n", buffer);
}

//************************************************************************
//
// Modified readCommand to allow getpass to be used instead of stdin
// 
//************************************************************************

void readPasswd(char *buffer, const char *salt, char *username) 
{
  char passwdPrompt[] = "Password [6-13 chars]: ";
  char* ps_buffer = getpass(passwdPrompt);
  int strLen = strlen(ps_buffer) - 1;
  if(strLen < MAX_PASSWD && strLen >= MIN_INPUT  && (strcmp(username, ps_buffer) != 0))
  {
    ps_buffer = crypt(ps_buffer, salt);
    buffer[strlen(ps_buffer)-1] = '\0';  // overwrite the line feed with null term
    strcpy(buffer, ps_buffer);
  }
  else if(strLen >= MAX_PASSWD)
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
  readCommand(cl_buffer);
  
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
        return true;
      }
      else
      {
        // Wrong password
        int err_login = 0;
        while(err_login < 3)
        {
          // Loop allows for a retry 3 times
          printf("Wrong password. Retry\n\t ");
          readPasswd(cl_buffer, cur_list[MAX_USERS - iterator].user_passwd, cur_list[MAX_USERS - iterator].user_name);
          // Auto exit if correct
          if(strcmp(cur_list[MAX_USERS - iterator].user_passwd,cl_buffer) == 0)
          {
            current_user = MAX_USERS - iterator;
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
	if((passphrase = fopen(PASSPHRASE, "r+")) == NULL)
	{
		printf("File not found.\n");
		exit(-1);
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
    return false;
  }
  
  
  // ELSE make account
  printf("Creating a new Account for USER[%d]:\n", current_user);
  // Prompt
  
  printf("Username[%d-%d chars]: ", MIN_INPUT, MAX_NAME - 1);
  readCommand(cl_buffer);
  int nameSize = strlen(cl_buffer);
  if(nameSize < MAX_NAME && nameSize >= MIN_INPUT)
  {
    // Search if username already taken
    for(int iterator = MAX_USERS; iterator > 0; iterator--)
    {
      // If successful create account
      if(strcmp(cur_list[MAX_USERS - iterator].user_name, cl_buffer) == 0)
      {
        printf("Error: Username not available\n");
        current_user = -1;
        return false;
      }
    } // search end
    strcpy(cur_list[current_user].user_name, cl_buffer);
  }
  else if(nameSize >= MAX_NAME)
  {
    printf("Error: Username too long\n");
    return false;
  }
  else if(nameSize < MIN_INPUT)
  {
    printf("Error: Username too short\n");
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
		fseek(passphrase, 0, SEEK_SET);
		int totalWritten = fwrite((unsigned char*) cur_list, sizeof(unsigned char), sizeof(users_t)*(current_user + 1), passphrase);
		printf("Wrote %d users out\n", totalWritten);
		// Read in users from file, so login can work next time 
    for(int iterator = MAX_USERS; iterator > 0; iterator--)
    {
      INIT_USER(cur_list[iterator - 1]);
    }
    printf("Current user: %s\n", cur_list[current_user].user_name);
    fseek(passphrase, 0, SEEK_SET);
    int totalBytes = fread((unsigned char*) cur_list, sizeof(unsigned char), sizeof(users_t)*MAX_USERS, passphrase);
    printf("Total bytes read in system: %d\n", totalBytes);
    printf("Current user: %s\n", cur_list[current_user].user_name);
    fclose(passphrase);
    // Makes sure users are only in memory
		
		return true;
  }
  else 
  {
    printf("Error: Could not create user. Password error in length or same as username\n");
    INIT_USER(cur_list[current_user]);
    current_user = -1;
		return false;
  }
}

void loginGetUsername(char curName[MAX_NAME])
{
  FILE* passphrase;
  users_t session_users[MAX_USERS];
	if((passphrase = fopen(PASSPHRASE, "r+")) == NULL)
	{
		printf("File not found.\n");
		exit(-1);
	}
  // Defaults for users set first
  for(int iterator = MAX_USERS; iterator > 0; iterator--)
  {
    INIT_USER(session_users[iterator - 1]);
  }
  
	// Read in users from file
	fseek(passphrase, 0, SEEK_SET);
	int totalBytes = fread((unsigned char*) session_users, sizeof(unsigned char), sizeof(users_t)*MAX_USERS, passphrase);
	fclose(passphrase);
	// Makes sure users are only in memory
  strcpy(curName, session_users[current_user].user_name);
  return;
}