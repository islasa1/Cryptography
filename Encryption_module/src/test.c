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
	
	char userInput = '\0';
	
	scanf(" %c", &userInput);
	
	while(userInput != 'q')
	{
		switch(userInput)
		{
			case 	'1':
				if((testFile = fopen("test.txt", "r+")) == NULL)
				{
					printf("File not found\n");
					break;
				}
				if((outputFile = fopen("test.txt.crpt", "w+")) == NULL)
				{
					printf("Could not open output file\n");
					break;
				}
				if((decryptFile = fopen("testDecrpt.txt", "w+")) == NULL)
				{
					printf("Could not open decrypt file\n");
					break;
				}

				encrypt(testFile, outputFile, default_key, MODE_TEXT);
				decrypt(outputFile, decryptFile, default_key, MODE_TEXT);

				fclose(testFile);
				fclose(outputFile);
				fclose(decryptFile);
				
				printf("Done!\n");
				
				break;
			case 	'2':
				if((testFile = fopen("large.txt", "r+")) == NULL)
				{
					printf("File not found\n");
					break;
				}
				if((outputFile = fopen("large.txt.crpt", "w+")) == NULL)
				{
					printf("Could not open output file\n");
					break;
				}
				if((decryptFile = fopen("largeDecrpt.txt", "w+")) == NULL)
				{
					printf("Could not open decrypt file\n");
					break;
				}

				encrypt(testFile, outputFile, default_key, MODE_TEXT);
				decrypt(outputFile, decryptFile, default_key, MODE_TEXT);

				fclose(testFile);
				fclose(outputFile);
				fclose(decryptFile);
				
				printf("Done!\n");
				
				break;
			case 	'3':
				if((testFile = fopen("test.bin", "r+")) == NULL)
				{
					printf("File not found\n");
					break;
				}
				if((outputFile = fopen("test.bin.crpt", "w+")) == NULL)
				{
					printf("Could not open output file\n");
					break;
				}
				if((decryptFile = fopen("testDecrpt.bin", "w+")) == NULL)
				{
					printf("Could not open decrypt file\n");
					break;
				}

				encrypt(testFile, outputFile, default_key, MODE_BINARY);
				decrypt(outputFile, decryptFile, default_key, MODE_BINARY);

				fclose(testFile);
				fclose(outputFile);
				fclose(decryptFile);
				
				printf("Done!\n");
				
				break;
			case 	'4':
				if((testFile = fopen("large.bin", "r+")) == NULL)
				{
					printf("File not found\n");
					break;
				}
				if((outputFile = fopen("large.bin.crpt", "w+")) == NULL)
				{
					printf("Could not open output file\n");
					break;
				}
				if((decryptFile = fopen("largeDecrpt.bin", "w+")) == NULL)
				{
					printf("Could not open decrypt file\n");
					break;
				}

				encrypt(testFile, outputFile, default_key, MODE_BINARY);
				decrypt(outputFile, decryptFile, default_key, MODE_BINARY);

				fclose(testFile);
				fclose(outputFile);
				fclose(decryptFile);
				
				printf("Done!\n");
				
				break;
			case 	'5':
				if((testFile = fopen("oddByte.txt", "r+")) == NULL)
				{
					printf("File not found\n");
					break;
				}
				if((outputFile = fopen("oddByte.txt.crpt", "w+")) == NULL)
				{
					printf("Could not open output file\n");
					break;
				}
				if((decryptFile = fopen("oddByteDecrpt.txt", "w+")) == NULL)
				{
					printf("Could not open decrypt file\n");
					break;
				}

				encrypt(testFile, outputFile, default_key, MODE_TEXT);
				decrypt(outputFile, decryptFile, default_key, MODE_TEXT);

				fclose(testFile);
				fclose(outputFile);
				fclose(decryptFile);
				
				printf("Done!\n");
				
				break;
			case '6':
				if((testFile = fopen("test.txt", "r+")) == NULL)
				{
					printf("File not found\n");
					break;
				}
				if((outputFile = fopen("test.txt.crpt", "w+")) == NULL)
				{
					printf("Could not open output file\n");
					break;
				}
				if((decryptFile = fopen("testDecrpt.txt", "w+")) == NULL)
				{
					printf("Could not open decrypt file\n");
					break;
				}

				encrypt(testFile, outputFile, default_key, MODE_BINARY);
				decrypt(outputFile, decryptFile, default_key, MODE_TEXT);

				fclose(testFile);
				fclose(outputFile);
				fclose(decryptFile);
				
				printf("Done!\n");
				
				break;
			case '7':
				if((testFile = fopen("test.bin", "r+")) == NULL)
				{
					printf("File not found\n");
					break;
				}
				if((outputFile = fopen("test.bin.crpt", "w+")) == NULL)
				{
					printf("Could not open output file\n");
					break;
				}
				if((decryptFile = fopen("testDecrpt.bin", "w+")) == NULL)
				{
					printf("Could not open decrypt file\n");
					break;
				}

				encrypt(testFile, outputFile, default_key, MODE_TEXT);
				decrypt(outputFile, decryptFile, default_key, MODE_TEXT);

				fclose(testFile);
				fclose(outputFile);
				fclose(decryptFile);
				
				printf("Done!\n");
				
				break;
			default:
				printf("Option not found\n");
				break;
		}
		
		scanf(" %c", &userInput);
	}
	
	return 0;
}
