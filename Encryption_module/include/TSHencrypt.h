#ifndef TSHENCRYPT_H
#define TSHENCRPYT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "error_handler.h"

#define MODE_BINARY  0
#define MODE_TEXT    1

bool encrypt(FILE *, FILE *, const unsigned int[2][2], bool);
bool decrypt(FILE *, FILE *, const unsigned int[2][2], bool);

#endif
