#ifndef BIGNUMS_H
#define BIGNUMS_H

#define MAX_LEN 42
#define MAX_RESULT 42
#define ERROR_DIVISION 2

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct
{
    bool negative;
    char mantiss[MAX_LEN];
    int grade;
}bignum_t;

#define MAX_GRADE 99999

#define ERROR_OVERFLOW 3

char* strip_leading_zeros(char *s);
void strip_trailing_zeros(char *s);
int compare_strings(const char* a, const char* b);
void subtract_strings(char* a, const char* b);
void multiply_by_digit(const char* num, int digit, char* result);
int divide_strings(const char* int_str, const char* double_str, char* result);
int big_division(bignum_t num1, bignum_t num2, bignum_t *res);

#endif
