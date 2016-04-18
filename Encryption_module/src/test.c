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
	
	unsigned int key[2][2] = {{105, 115}, {108, 97}};
	unsigned int keyInverse[2][2];
	printf("Determinant (Z) : %u\n", convertZ(1, 105*97 - 108*115));
	
	inverse2_Z(key, keyInverse);
	
	// Debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 2; j++)
			printf("%u\t", key[i][j]);
	printf("\n");
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 2; j++)
			printf("%u\t", keyInverse[i][j]);
	printf("\n");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	return 0;
}
