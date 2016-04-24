#include <stdio.h>

#include "shell_utils.c"

#define TAG "myTag"
#define NOT_TAG "notMyTag"

int main(void)
{
  FILE* input;
  char userInput = '\0';
  
  scanf(" %c", &userInput);
  
  while(userInput != 'q')
  { 
    switch(userInput)
    {
      case 't':
        if((input = fopen("test.txt", "r+")) == NULL)
        {
          printf("Could not create file.\n");
          exit(-1);
        }
        fseek(input, 0, SEEK_END);
        tagFile(input, TAG) ? printf("Tagged file!\n") : printf("Unable to tag file.\n");
        fseek(input, 0, SEEK_END);
        fclose(input);
        break;
      case 'c':
        if((input = fopen("test.txt", "r+")) == NULL)
        {
          printf("Could not create file.\n");
          exit(-1);
        }
        fseek(input, 0, SEEK_END);
        checkTag(input, TAG) ? printf("Tag found!\n") : printf("Could not find tag.\n");
        fseek(input, 0, SEEK_END);
        fclose(input);
        break;
      case 'k': {
        loginSetCurUser(0);

        unsigned int myKey[2][2];
        getKey(myKey) ? \
          printf("Created key:\n%u\t%u\n%u\t%u\n", myKey[0][0], myKey[0][1], myKey[1][0], myKey[1][1]) : printf("Could not make key.\n");
        break;
      }
      case '1':
        input = NULL;
        tagFile(input, TAG) ? printf("Tagged file!\n") : printf("Unable to tag file.\n");
        break;
      case '2':
        if((input = fopen("test.txt", "r+")) == NULL)
        {
          printf("Could not create file.\n");
          exit(-1);
        }
        fseek(input, 0, SEEK_END);
        checkTag(input, NOT_TAG) ? printf("Tag found!\n") : printf("Could not find tag.\n");
        fseek(input, 0, SEEK_END);
        fclose(input);
        break;
      case '3': {
        loginSetCurUser(-1);

        unsigned int myKey[2][2];
        getKey(myKey) ? \
          printf("Created key:\n%u\t%u\n%u\t%u\n", myKey[0][0], myKey[0][1], myKey[1][0], myKey[1][1]) : printf("Could not make key.\n");
        break;
      }
    }
    scanf(" %c", &userInput);
  }
  
  
  printf("Compiled!\n");
}