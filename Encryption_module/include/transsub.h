#ifndef TRANSSUB_H
#define TRANSSUB_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "utilities.h"
#include "error_handler.h"

unsigned char findbeta(unsigned char);
unsigned char findalpha(unsigned char);
bool transpose(FILE *, FILE *, int);
bool detranspose(FILE *, FILE *, int);
void maketransmap(int, int);

#endif
