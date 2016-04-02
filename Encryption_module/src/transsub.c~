#include "transsub.h"

int *transmap;
int *detransmap;

#define ALPHABET (26)

struct charmap
{
    char alpha;
    char beta;
};

struct charmap submap[ALPHABET] =
{
    {'A','N'}, {'B','A'}, {'C','S'}, {'D','I'}, {'E','J'}, {'F','K'}, {'G','C'},
    {'H','M'}, {'I','Q'}, {'J','R'}, {'K','F'}, {'L','B'}, {'M','D'}, {'N','G'},
    {'O','H'}, {'P','E'}, {'Q','L'}, {'R','O'}, {'S','P'}, {'T','W'}, {'U','T'},
    {'V','Z'}, {'W','Y'}, {'X','V'}, {'Y','U'}, {'Z','X'}
};

// Encode with substitution
char findbeta(char alpha)
{
    int i;

    for(i=0; i < ALPHABET; i++)
    {
        if(submap[i].alpha == alpha)
            return(submap[i].beta);
    }

    return alpha;
}

// Decode with substitution
char findalpha(char beta)
{
    int i;

    for(i=0; i < ALPHABET; i++)
    {
        if(submap[i].beta == beta)
            return(submap[i].alpha);
    }
    return beta;
}

// Encode with transposition 2 files
void transpose(FILE *input, FILE *output, int length)
{
    int i, offset = ftell(input);
    
    for(i=0; i < length; i++)
    {
	fseek(input, transmap[i], SEEK_SET);
        fputc(fgetc(input), output);
    }
	
	fseek(input, offset, SEEK_SET);
}

// Decode with transposition 2 files
void detranspose(FILE *input, FILE *output, int length)
{
    int i, offset = ftell(input);

    for(i=0; i < length; i++)
    {
        fseek(input, detransmap[i], SEEK_SET);
        fputc(fgetc(input), output);
    }
	
	fseek(input, offset, SEEK_SET);
}

// Dynamically make the transmap and detransmap
void maketransmap(int key, int size)
{
	int i;
	for(i = 0; i < size; i++)
		transmap[i] = i;
	
	shuffle(transmap, size, key);
	
	for(i = 0; i < size; i++)
		detransmap[transmap[i]] = i;
}

