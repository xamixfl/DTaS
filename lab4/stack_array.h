#ifndef _STACK_ARRAY_H
#define _STACK_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "main_processing.h"

#define ARR_STACK_MAX_SIZE 1000

typedef struct
{
    char data[ARR_STACK_MAX_SIZE];
    int top;
} stack_arr_t;

void stack_array_init(stack_arr_t *s);
int stack_array_is_empty(const stack_arr_t *s);
int stack_array_is_full(const stack_arr_t *s);
int stack_array_push(stack_arr_t *s, char val);
int stack_array_pop(stack_arr_t *s, char *val);
void stack_array_print(const stack_arr_t *s);

#endif
