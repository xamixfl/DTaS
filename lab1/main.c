/*
Смоделировать операцию деления целого числа длиной до
40 десятичных цифр на действительное число в форме
±m.n Е K, где суммарная длина мантиссы (m+n) - до 40
значащих цифр, а величина порядка K - до 5 цифр.
Результат выдать в форме ±0.m1 Е ±K1, где m1 - до 40
значащих цифр, а K1 - до 5 цифр.
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>

#include "io.h"
#include "bignums.h"

int main(void)
{
    printf("Введите 2 числа : целое - делимое и действительное - делитель.\n"
        ">+---------1---------2---------3---------4---------5\n");

    char intnum_str[MAX_LEN];
    bool is_neg_int;
    int rc = input_int(intnum_str, &is_neg_int);
    if (rc != 0)
        return rc;

    char mantiss[MAX_LEN + 1];
    bool is_neg_fl;
    int grade;
    
    rc = input_float(mantiss, &grade, &is_neg_fl);
    if (rc != 0)
        return rc;

    bignum_t intnum;
    intnum.negative = is_neg_int;
    strncpy(intnum.mantiss, intnum_str, MAX_LEN);
    intnum.grade = 0;

    bignum_t floatnum;
    floatnum.negative = is_neg_fl;
    strncpy(floatnum.mantiss, mantiss, MAX_LEN);
    floatnum.grade = grade;
    
    bignum_t result;
    result.grade = 0;
    rc = big_division(intnum, floatnum, &result);
    if (rc != 0)
    {
        return ERROR_DIVISION;
    }

    print_result(result);

    return 0;
}

