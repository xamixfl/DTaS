#include "stack_list.h"

void free_list_init(FreeList_t *list)
{
    list->head = NULL;
}

int free_list_node_add(FreeList_t *list, void *address)
{
    FreeNode_t *node = malloc(sizeof(FreeNode_t));
    if (node == NULL)
        return ERROR_STACK_MALLOC;
    node->address = address;
    node->next = list->head;
    list->head = node;
    return 0;
}

void free_list_print(FreeList_t *list)
{
    if (list->head == NULL)
    {
        printf("Список адресов пуст.\n");
        return;
    }
    printf("Список свободных адресов: \n");
    FreeNode_t *cur = list->head;
    while (cur != NULL)
    {
        printf(" %p\n", cur->address);
        cur = cur->next;
    }
}

void free_list_free(FreeList_t *list)
{
    FreeNode_t*cur = list->head;
    while (cur != NULL)
    {
        FreeNode_t *temp = cur;
        cur = cur->next;
        free(temp);
    }
    list->head = NULL;
}

void list_stack_init(ListStack_t *s)
{
    s->top = NULL;
    s->size = 0;
}

int list_stack_is_empty(const ListStack_t *s)
{
    int res = s->top == NULL;
    return res;
}

int list_stack_is_full(const ListStack_t *s)
{
    int res = s->size >= LIST_STACK_MAX_SIZE;
    return res;
}

int list_stack_push(ListStack_t *s, char value)
{
    if (list_stack_is_full(s))
        return ERROR_STACK_FULL;
    Node_t *node = malloc(sizeof(Node_t));
    node->data = value;
    node->next = s->top;
    s->top = node;
    s->size++;

    return 0;
}

int list_stack_pop(ListStack_t *s, char *val, FreeList_t *freelist)
{
    if (list_stack_is_empty(s))
        return ERROR_STACK_EMPTY;

    Node_t *temp = s->top;
    *val = temp->data;
    s->top = temp->next;
    s->size--;

    if (freelist != NULL) 
    {
        int rc = free_list_node_add(freelist, temp);
        if (rc) 
            return rc;
    }
    free(temp);
    return 0;
}

void list_stack_print(const ListStack_t *s)
{
    if (list_stack_is_empty(s))
    {
        printf("Стек пуст.\n");
        return;
    }
    printf("Стек, в порядке от вершины: \n");
    Node_t *cur = s->top;
    while (cur != NULL)
    {
        printf(" Значение: %c; Адрес: %p\n", cur->data, cur);
        cur = cur->next;
    }
}

void list_stack_free(ListStack_t *s)
{
    Node_t *cur = s->top;
    while (cur != NULL)
    {
        Node_t *temp = cur;
        cur = cur->next;
        free(temp);
    }
    s->top = NULL;
    s->size = 0;
}
