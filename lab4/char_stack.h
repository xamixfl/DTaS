#ifndef _CHAR_STACK_H_
#define _CHAR_STACK_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHAR_STACK_MAX_SIZE 1000

typedef struct
{
    char data[CHAR_STACK_MAX_SIZE];
    int top;
} CharStack_t;

void char_stack_init(CharStack_t *s);
bool char_stack_is_empty(CharStack_t *s);
bool char_stack_push(CharStack_t *s, char c);
bool char_stack_pop(CharStack_t *s, char *c);
void char_stack_free(CharStack_t* s);

#endif
