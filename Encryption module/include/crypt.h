#ifndef CRYPT_H
#define CRYPT_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "utilities.h"

char findbeta(char);
char findalpha(char);
void transpose(FILE *, FILE *, int);
void detranspose(FILE *, FILE *, int);
void maketransmap(int, int);

#endif
