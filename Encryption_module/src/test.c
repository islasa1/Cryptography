#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define USE_DEF_KEY2

extern const unsigned int default_key[2][2];
extern const unsigned int default_keyInverse[2][2];

#include "hillcipher.h"
#include "TSHencrypt.h"

int main(void)
{
	FILE *testFile, *outputFile, *decryptFile;
	if((testFile = fopen("test.txt", "r+")) == NULL)
	{
		printf("File not found\n");
		exit(-1);
	};
	if((outputFile = fopen("testEn.txt", "w+")) == NULL)
	{
		printf("Could not open output file\n");
		exit(-1);
	}
	if((decryptFile = fopen("testDecrpt.txt", "w+")) == NULL)
	{
		printf("Could not open decrypt file\n");
		exit(-1);
	}
	
	encrypt(testFile, outputFile, default_key);
	decrypt(outputFile, decryptFile, default_key);
	
	fclose(testFile);
	fclose(outputFile);
	fclose(decryptFile);
	
	return 0;
}
