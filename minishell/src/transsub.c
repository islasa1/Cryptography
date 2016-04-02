#include "transsub.h"

int *transmap;
int *detransmap;

#define ALPHABET (91)

struct charmap
{
    char alpha;
    char beta;
};

struct charmap submap[ALPHABET] =
{
    {'A','N'}, {'B',']'}, {'C','1'}, {'D','A'}, {'E','*'}, {'F',','}, {'G','u'},
    {'H','I'}, {'I','$'}, {'J','5'}, {'K','l'}, {'L','Y'}, {'M','`'}, {'N','d'},
    {'O','?'}, {'P','Q'}, {'Q','4'}, {'R','('}, {'S','t'}, {'T','K'}, {'U','+'},
    {'V','\\'}, {'W','v'}, {'X','7'}, {'Y','C'}, {'Z','@'}, {'a','X'}, {'b','^'},
    {'c','f'}, {'d','!'}, {'e','U'}, {'f','\"'}, {'g','z'}, {'h','9'}, {'i','.'},
    {'j','D'}, {'k','a'}, {'l','/'}, {'m','b'}, {'n','['}, {'o','3'}, {'p',':'},
    {'q','m'}, {'r','S'}, {'s','<'}, {'t','2'}, {'u','G'}, {'v','-'}, {'w','B'},
    {'x','h'}, {'y','='}, {'z','P'}, {'+','{'}, {'-','r'}, {'*','y'}, {'/','~'},
    {'=','R'}, {'&','8'}, {'!',')'}, {'?','c'}, {'.','E'}, {',','\''}, {'(','Z'},
    {')','s'}, {'{','>'}, {'}','i'}, {'[','J'}, {']','o'}, {'\'','w'}, {'\"','&'},
    {'\\','e'}, {'1','L'}, {'2','6'}, {'3','q'}, {'4','V'}, {'5','}'}, {'6','x'},
    {'7','g'},{'8',';'}, {'9','p'}, {'0','#'}, {'~','j'}, {'$','T'}, {'^','W'},
    {'<','O'}, {'>','0'}, {'#','k'}, {'@','n'}, {'`','F'}, {';','M'}, {':','H'},
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

