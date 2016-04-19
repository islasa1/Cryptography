#ifndef TRANSSUB_H
#define TRANSSUB_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "utilities.h"
#include "error_handler.h"

char findbeta(char);
char findalpha(char);
void transpose(FILE *, FILE *, int);
void detranspose(FILE *, FILE *, int);
void maketransmap(int, int);

#endif
