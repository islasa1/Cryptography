#include "TSHencrypt.h"
#include "hillcipher.h"
#include "transsub.h"

extern int *transmap;
extern int *detransmap;

void encrypt(FILE *input, FILE *output, const unsigned int key[2][2])
{
	// Check for proper inputs
	if(input == NULL)
	{
		printf("Input file null");
		return;
	}
	
	if (output == NULL)
	{
		printf("Output file null");
		return;
	}
	
	// Calculate the size of the file
	fseek(input, 0L, SEEK_END);
	int size = ftell(input);
	fseek(input, 0L, SEEK_SET);
	
	// Transposition
	int determinant = key[0][0] * key[1][1] - key[0][1] * key[1][0];
	transmap = (int *)malloc(sizeof(int) * size);
	detransmap = (int *)malloc(sizeof(int) * size);
	maketransmap(determinant, size);
	transpose(input, output, size);
	free((void *)transmap);
	free((void *)detransmap);
	
	// Substitution
	int i;
	char temp;
	fseek(output, 0L, SEEK_SET);
	for(i = 0; i < size; i++)
	{
		temp = findbeta((char)fgetc(output));
		fseek(output, -1, SEEK_CUR);
		fputc(temp, output);
	}
	
	// Hill cipher
	fseek(output, 0L, SEEK_SET);
	unsigned int cipherin[2][1], cipherout[2][1];
	
	for(i = 0; i < size; i +=2)
	{
		cipherin[0][0] = (unsigned int)fgetc(output);
		cipherin[1][0] = (unsigned int)fgetc(output);
		
		// Use psuedo-EOF ' ' to use an unsigned int in hillcipher (stay in Z_PRIME)
		if(cipherin[1][0] == EOF) cipherin[1][0] = 0;
		
		hillcipher2(cipherin, cipherout, key);
		
		// Very careful about seek since we are going by 2's
		// The output file will always be an even number of bytes
		// going up for odd byte values (e.g. 15 bytes will output 16)
		// This is so the hillcipher decryption will use an encryped EOF
		// to decrypt the last byte
		if(cipherin[1][0] != 0)
			fseek(output, -2, SEEK_CUR);
		else 
			fseek(output, -1, SEEK_CUR);
		
		// Always output both for encryption
		fputc((unsigned char)cipherout[0][0], output);
		fputc((char)cipherout[1][0], output);
	}
}

void decrypt(FILE *input, FILE *output, const unsigned int key[2][2])
{
	// Check for proper inputs
	if(input == NULL)
	{
		printf("Input file null");
		return;
	}
	
	if (output == NULL)
	{
		printf("Output file null");
		return;
	}
	
	// Calculate the size of the file
	fseek(input, 0L, SEEK_END);
	int size = ftell(input);
	fseek(input, 0L, SEEK_SET);
	
	// Hill cipher
	int i;
	fseek(output, 0L, SEEK_SET);
	unsigned int cipherin[2][1], cipherout[2][1], inverseKey[2][2];
	
	if(!inverse2_Z( (unsigned int (*)[2]) key, inverseKey)) 
	{
		printf("Aborting decryption: Key non-invertable\n");
		return;
	}
	if(!checkKeyIntegrity2(key, (const unsigned int (*)[2])inverseKey))
	{
		printf("Aborting decryption: Bad key\n");	
		return;
	}
	
	for(i = 0; i < size; i +=2)
	{
		cipherin[0][0] = (unsigned int)fgetc(input);
		cipherin[1][0] = (unsigned int)fgetc(input);
		hillcipher2(cipherin, cipherout, (const unsigned int (*)[2]) inverseKey);
		
		// Very careful.. don't put EOF for decryption, corrected back to
		// odd bytes if original was odd, check for psuedo-EOF
		if(cipherout[1][0] != 0)
		{
			fputc((unsigned char)cipherout[0][0], output);
			fputc((char)cipherout[1][0], output);
		}
		else 
		{
			fputc((unsigned char)cipherout[0][0], output);
			size--;
		}
	}
	
	// Substitution
	FILE *transTemp;
	transTemp = fopen("temp.txt", "w+");
	fseek(output, 0L, SEEK_SET);
	for(i = 0; i < size; i++)
	{
		fputc(findalpha((char)fgetc(output)), transTemp);
	}
	
	// Transposition
	fseek(output, 0L, SEEK_SET);
	int determinant = key[0][0] * key[1][1] - key[0][1] * key[1][0];
	transmap = (int *)malloc(sizeof(int) * size);
	detransmap = (int *)malloc(sizeof(int) * size);
	maketransmap(determinant, size);
	detranspose(transTemp, output, size);
	free((void *)transmap);
	free((void *)detransmap);

	// destroy temp file
	fclose(transTemp);
	remove("temp.txt");

}
