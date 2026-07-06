#include "char_stack.h"

void char_stack_init(CharStack_t *s) 
{
    s->top = -1;
}

bool char_stack_is_empty(CharStack_t *s) 
{
    return s->top == -1;
}

bool char_stack_push(CharStack_t *s, char c) 
{
    if (s->top >= CHAR_STACK_MAX_SIZE - 1) return false;
    s->data[++s->top] = c;
    return true;
}

bool char_stack_pop(CharStack_t *s, char *c) 
{
    if (char_stack_is_empty(s)) return false;
    *c = s->data[s->top--];
    return true;
}

void char_stack_free(CharStack_t* s)
{
    if (s == NULL) return;
    s->top = -1; 
}
