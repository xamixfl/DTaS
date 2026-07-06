#include "stack_array.h"

void stack_array_init(stack_arr_t *s)
{
    s->top = -1;
}

int stack_array_is_empty(const stack_arr_t *s)
{
    return s->top == -1;
}

int stack_array_is_full(const stack_arr_t *s)
{
    return s->top == ARR_STACK_MAX_SIZE - 1;
}

int stack_array_push(stack_arr_t *s, char val)
{
    if (stack_array_is_full(s))
        return ERROR_STACK_FULL;
    s->top++;
    s->data[s->top] = val;

    return 0;
}

int stack_array_pop(stack_arr_t *s, char *val)
{
    if (stack_array_is_empty(s))
        return ERROR_STACK_EMPTY;
    *val = s->data[s->top];
    s->top--;

    return 0;
}

void stack_array_print(const stack_arr_t *s)
{
    if (stack_array_is_empty(s))
    {
        printf("Стек пуст.\n");
        return;
    }
    printf("Массив стека от вершины к основанию:\n");
    for (int i = s->top; i >= 0; i--)
        printf("  %c\n", s->data[i]);
}



