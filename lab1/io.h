#ifndef IO_H
#define IO_H

#include "bignums.h"

#define ERROR_INPUT 1
#define ERROR_DIVISION 2

#define MAX_LEN 42

int input_int(char *intnum, bool *neg);
int input_float(char *mantiss, int *grade, bool *neg);
void print_result(bignum_t result);

#endif
