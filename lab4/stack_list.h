#ifndef _STACK_LIST_H_
#define _STACK_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "main_processing.h"

#define LIST_STACK_MAX_SIZE 1000

typedef struct Node_t
{
    char data;
    struct Node_t *next;
} Node_t;

typedef struct 
{
    Node_t *top;
    int size;
} ListStack_t;

typedef struct FreeNode_t FreeNode_t;
struct FreeNode_t
{
    void* address;
    FreeNode_t* next;
};

typedef struct 
{
    FreeNode_t *head;
} FreeList_t;

void free_list_init(FreeList_t *list);
int free_list_node_add(FreeList_t *list, void *address);
void free_list_print(FreeList_t *list);
void free_list_free(FreeList_t *list);
void list_stack_init(ListStack_t *s);
int list_stack_is_empty(const ListStack_t *s);
int list_stack_is_full(const ListStack_t *s);
int list_stack_push(ListStack_t *s, char value);
int list_stack_pop(ListStack_t *s, char *value, FreeList_t *free_list);
void list_stack_print(const ListStack_t *s);    
void list_stack_free(ListStack_t *s);

#endif
