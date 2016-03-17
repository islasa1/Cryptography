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
	
	printf("Test inverse of a 3x3 matrix in Z_PRIME field\n");
	unsigned int testM[3][3] = {{'h', 'e', 'l'}, {'l', 'o', 'y'}, {'o', 'u', '!'}};
	unsigned int inverseM[3][3];
	
	if(!inverse3_Z(testM, inverseM)) exit(1);
	
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			printf("%d ", testM[i][j]);
		}
		printf("\n");
	}
	
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			printf("%d ", inverseM[i][j]);
		}
		printf("\n");
	}
	
	if(checkKeyIntegrity3((const unsigned int (*)[3]) testM,
												(const unsigned int (*)[3]) inverseM)) printf("Inverse OK!\n");
	
	return 0;
}
