// Shell utilities for minishell
// Left here as independent module for explicit testing
// and possibility of being used elsewhere besides the shell

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include    "login.h"

#define ASCII_RANGE  94 
#define ASCII_BASE   32

//*****************************************************************************
//
// User Key Gen
// Preconditions: User is logged in, and an unsigned int 2x2 matrix is passed in
// Postconditions: A Z_Prime invertable 2x2 matrix is returned, true if completed
//*****************************************************************************
bool getKey(unsigned int key[2][2])
{
  if(loginGetCurUser() == -1)
  {
    return false; 
  }
  char* username = loginGetUsername();
  // A username must be between 6-8 chars, and are inside promptString
  key[0][0] = username[0];
  key[0][1] = username[1];
  key[1][0] = username[2];
  key[1][1] = username[3];
  
  // We are assuming user has only normal ASCII chars available
  int det = key[0][0]*key[1][1] - key[0][1]*key[1][0];
  
  if(det == 0)
  {
    // Flip some numbers to find a new matrix
    key[0][0] = key[0][1];
    key[0][1] = username[0];
    // Recalculate determinant
    det = key[0][0]*key[1][1] - key[0][1]*key[1][0];
  }
  else return true;
  
  if(det == 0)
  {
    // Seed the psuedo-random number generator to 
    // get a reproducible key
    srand(det);
    while(det == 0)
    {
      key[0][0] = rand() % ASCII_RANGE + ASCII_BASE;
      key[0][1] = rand() % ASCII_RANGE + ASCII_BASE;
      key[1][0] = rand() % ASCII_RANGE + ASCII_BASE;
      key[1][1] = rand() % ASCII_RANGE + ASCII_BASE;
      
      det = key[0][0]*key[1][1] - key[0][1]*key[1][0];
    }
  }
  
  return true;
}

//*****************************************************************************
//
// File Tagging
// Preconditions: FILE* points to a file already opened for read and write priveleges
//                and a valid char* to a tag is passed in
// Postconditions: The file has been tagged, input FILE* will not be closed
//*****************************************************************************
bool tagFile(FILE* file, char* tag)
{
  // Check for proper inputs
	if(file == NULL)
	{
		printf("Input file null\n");
		return false;
	}
  else if(tag == NULL)
  {
		printf("Invalid tag\n");
		return false;
	}
  
  fseek(file, 0L, SEEK_END);
  int length = strlen(tag);
  for(int i = 0; i < length; i++)
  {
    fputc(tag[i], file); 
  }
  
  return true;
  
}

//*****************************************************************************
//
// Check File Tagging
// Preconditions: FILE* points to a file already opened for read and write priveleges
//                and a valid char* to a tag is passed in
// Postconditions: The file has tag removed and returns true
//*****************************************************************************
bool checkTag(FILE* file, char* tag)
{
  // Check for proper inputs
	if(file == NULL)
	{
		printf("Input file null");
		return false;
	}
  else if(tag == NULL)
  {
		printf("Invalid tag\n");
		return false;
	}
  
  int length = strlen(tag);
  if(fseek(file, -(length), SEEK_END) != 0) 
  {
    printf("Seek error\n");
    return false;
  }
  char compareTag[length + 1];
  memset(compareTag, 0, length + 1);
  
  for(int i = 0; i < length; i++)
  {
    compareTag[i] = (char) fgetc(file); 
  }
  
  
  if(strcmp(tag, compareTag) == 0)
  {
    // NOTE: Since we are writing EOF manually to file
    // immediately close and reopen the file if need be 
    // when returning from checkTag()
    // Tag checks out!
    long int bytes;
    if(fseek(file, -(length), SEEK_END) != 0) 
    {
      printf("Seek error\n");
      return false;
    }
    if((bytes = ftell(file)) == -1)
    {
      printf("Ftell error\n");
      return false;
    }
    if(ftruncate(fileno(file), bytes) == -1)
    {
      printf("Ftruncate error\n");
      return false;
    }
    
    return true;
  }
  else return false;
}
