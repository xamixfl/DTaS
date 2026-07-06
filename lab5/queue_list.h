#ifndef _QUEUE_LIST_H_
#define _QUEUE_LIST_H_

#include <stdio.h>

#define LIST_MAX_SIZE 1000

#define ERROR_LIST_OVERFLOW -3
#define ERROR_EMPTY_LIST -4

extern int max_size_list;

typedef struct node
{
    int type;
    double arrival_time;
    struct node *next;
} node_t;

typedef struct
{
    node_t *pin;
    node_t *pout;
    int size;
} queue_list_t;

void queue_list_init(queue_list_t *q);
int queue_list_add(queue_list_t *q, int type);
int queue_list_del(queue_list_t *q, int *type);
int queue_list_insert_4th(queue_list_t *q, int type, node_t **new_node_ptr);

#endif
