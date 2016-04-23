#include "TSHencrypt.h"
#include "hillcipher.h"
#include "transsub.h"

bool encrypt(FILE *input, FILE *output, const unsigned int key[2][2], bool mode)
{
	// Check for proper inputs
	ERROR_PTR_BOOL(input);
	ERROR_PTR_BOOL(output);
	
	// Calculate the size of the file
	PERROR_NUM_BOOL(fseek(input, 0L, SEEK_END));
	long int size = ftell(input);
	PERROR_NUM_BOOL(fseek(input, 0L, SEEK_SET));
	
	// Transposition
	int determinant = (int) key[0][0] * key[1][1] - (int) key[0][1] * key[1][0];
	int *transmap = (int *)malloc(sizeof(int) * size);
	ERROR_PTR_BOOL(transmap);
	
	int *detransmap = (int *)malloc(sizeof(int) * size);
	ERROR_PTR_BOOL(detransmap);
	
	maketransmap(determinant, size, transmap, detransmap);
	
	// Do not use macro here, redundant failure then
	if(!transpose(input, output, size, transmap)) {
		printf("Error: Transposition failure.\n");
		return false;
	}
	free((void *)transmap);
	free((void *)detransmap);
	
	// Substitution
	int i;
	int temp;
	PERROR_NUM_BOOL(fseek(output, 0L, SEEK_SET));
	for(i = 0; i < size; i++)
	{
		ERROR_NUM_BOOL((temp = fgetc(output)));
		temp = (unsigned char) findbeta((unsigned char) temp);
		PERROR_NUM_BOOL(fseek(output, -1, SEEK_CUR));
		ERROR_NUM_BOOL(fputc(temp, output));
	}
	
	if(mode == MODE_TEXT)
	{
		// Hill cipher
		PERROR_NUM_BOOL(fseek(output, 0L, SEEK_SET));
		unsigned int cipherin[2][1], cipherout[2][1];
		int c0, c1;
		for(i = 0; i < size; i +=2)
		{	
			ERROR_NUM_BOOL((c0 = fgetc(output)));
			// Cannot check error on c1 because we could get EOF
			c1 = fgetc(output);

			cipherin[0][0] = (unsigned int) c0;
			cipherin[1][0] = (unsigned int) c1;

			// Use psuedo-EOF ' ' to use an unsigned int in hillcipher (stay in Z_PRIME)
			if(c1 == EOF)
			{
				cipherin[1][0] = 0;
			}

			hillcipher2(cipherin, cipherout, key);

			// Very careful about seek since we are going by 2's
			// The output file will always be an even number of bytes
			// going up for odd byte values (e.g. 15 bytes will output 16)
			// This is so the hillcipher decryption will use an encryped EOF
			// to decrypt the last byte
			if(c1 != EOF){
				PERROR_NUM_BOOL(fseek(output, -2, SEEK_CUR));
			}
			else {
				PERROR_NUM_BOOL(fseek(output, -1, SEEK_CUR));
			}

			// Always output two chars for encryption
			ERROR_NUM_BOOL(fputc((unsigned char)cipherout[0][0], output));
			ERROR_NUM_BOOL(fputc((unsigned char)cipherout[1][0], output));
		}
	}
	
	return true;
}

bool decrypt(FILE *input, FILE *output, const unsigned int key[2][2], bool mode)
{
	// Check for proper inputs
	ERROR_PTR_BOOL(input);
	ERROR_PTR_BOOL(output);
	
	// Calculate the size of the file
	PERROR_NUM_BOOL(fseek(input, 0L, SEEK_END));
	long int size = ftell(input);
	PERROR_NUM_BOOL(fseek(input, 0L, SEEK_SET));
	
	// Hill cipher
	int i;
	int c0;
	
	if(mode == MODE_TEXT)
	{
		int c1;

		PERROR_NUM_BOOL(fseek(output, 0L, SEEK_SET));
		unsigned int cipherin[2][1], cipherout[2][1], inverseKey[2][2];

		if(!inverse2_Z( (unsigned int (*)[2]) key, inverseKey)) 
		{
			printf("Aborting decryption: Key non-invertable\n");
			return false;
		}
		if(!checkKeyIntegrity2(key, (const unsigned int (*)[2])inverseKey))
		{
			printf("Aborting decryption: Bad key\n");	
			return false;
		}

		for(i = 0; i < size; i +=2)
		{
			ERROR_NUM_BOOL((c0 = fgetc(input)));
			ERROR_NUM_BOOL((c1 = fgetc(input)));
			cipherin[0][0] = (unsigned int) c0;
			cipherin[1][0] = (unsigned int) c1;
			hillcipher2(cipherin, cipherout, (const unsigned int (*)[2]) inverseKey);

			// Very careful.. don't put EOF for decryption, corrected back to
			// odd bytes if original was odd, check for psuedo-EOF
			if((size > i+2) || (cipherout[1][0]  != 0))
			{
				ERROR_NUM_BOOL(fputc((unsigned char)cipherout[0][0], output));
				ERROR_NUM_BOOL(fputc((unsigned char)cipherout[1][0], output));
			}
			else 
			{
				ERROR_NUM_BOOL(fputc((unsigned char)cipherout[0][0], output));
				size--;
			}
		}
	}

	// Substitution
	char tempFileName[16];
	memset((void *)tempFileName, 16, sizeof(char));
	int fileNum;
	PERROR_NUM_BOOL((fileNum = fileno(input)));
	sprintf(tempFileName, "temp%d.txt", fileNum);
	FILE *transTemp;
	transTemp = fopen(tempFileName, "w+");
	
	PERROR_PTR_BOOL(transTemp);
	PERROR_NUM_BOOL(fseek(output, 0L, SEEK_SET));
	PERROR_NUM_BOOL(fseek(transTemp, 0L, SEEK_SET));
	
	// Necessary for proper piping if mode is changed
	FILE* switchMode = (mode == MODE_TEXT) ? output : input;
	
	for(i = 0; i < size; i++)
	{
		ERROR_NUM_BOOL((c0 = fgetc(switchMode)));
		ERROR_NUM_BOOL(fputc(findalpha((unsigned char) c0), transTemp));
	}
	
	// Transposition
	PERROR_NUM_BOOL(fseek(output, 0L, SEEK_SET));
	int determinant = (int) key[0][0] * key[1][1] - (int) key[0][1] * key[1][0];
	int *transmap = (int *)malloc(sizeof(int) * size);
	ERROR_PTR_BOOL(transmap);
	
	int *detransmap = (int *)malloc(sizeof(int) * size);
	ERROR_PTR_BOOL(detransmap);
	
	maketransmap(determinant, size, transmap, detransmap);
	
	if(!detranspose(transTemp, output, size, detransmap)) {
		printf("Error: Detranspose error.\n");
		return false;
	}
	
	free((void *)transmap);
	free((void *)detransmap);

	// destroy temp file
	PERROR_NUM_BOOL(fclose(transTemp));
	PERROR_NUM_BOOL(remove(tempFileName));

	return true;
}
