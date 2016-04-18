#include <stdio.h>

#include "shell_utils.c"

#define TAG "myTag"

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
        printf("Bytes: %ld\n", ftell(input));
        tagFile(input, TAG) ? printf("Tagged file!\n") : printf("Unable to tag file.\n");
        fseek(input, 0, SEEK_END);
        printf("Bytes: %ld\n", ftell(input));
        fclose(input);
        break;
      case 'c':
        if((input = fopen("test.txt", "r+")) == NULL)
        {
          printf("Could not create file.\n");
          exit(-1);
        }
        fseek(input, 0, SEEK_END);
        printf("Bytes: %ld\n", ftell(input));
        checkTag(input, TAG) ? printf("Tag found!\n") : printf("Could not find tag.\n");
        fseek(input, 0, SEEK_END);
        printf("Bytes: %ld\n", ftell(input));
        fclose(input);
        break;
      case 'k':
        loginSetCurUser(0);

        unsigned int myKey[2][2];
        getKey(myKey) ? \
          printf("Created key:\n%u\t%u\n%u\t%u\n", myKey[0][0], myKey[0][1], myKey[1][0], myKey[1][1]) : printf("Could not make key.\n");
        break;
    }
    scanf(" %c", &userInput);
  }
  printf("Compiled!\n");
}