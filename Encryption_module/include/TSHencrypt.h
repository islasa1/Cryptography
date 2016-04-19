#ifndef TSHENCRYPT_H
#define TSHENCRPYT_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "error_handler.h"

void encrypt(FILE *, FILE *, const unsigned int[2][2]);
void decrypt(FILE *, FILE *, const unsigned int[2][2]);

#endif
