#include "queue_list.h"
#include <stdlib.h>

void queue_list_init(queue_list_t *q)
{
    q->size = 0;
    q->pin = NULL;
    q->pout = NULL;
}

int queue_list_add(queue_list_t *q, int type)
{
    if (q->size >= LIST_MAX_SIZE)
        return ERROR_LIST_OVERFLOW;
    
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (!new_node)
        return ERROR_LIST_OVERFLOW;
    
    new_node->type = type;
    new_node->arrival_time = 0;
    new_node->next = NULL;
    
    if (q->pin == NULL)
    {
        q->pin = new_node;
        q->pout = new_node;
    }
    else
    {
        q->pin->next = new_node;
        q->pin = new_node;
    }
    q->size++;
    if (q->size > max_size_list)
        max_size_list = q->size;

    return 0;
}

int queue_list_del(queue_list_t *q, int *type)
{
    if (q->size == 0)
        return ERROR_EMPTY_LIST;
    
    node_t *temp = q->pout;
    *type = temp->type; 
    
    q->pout = q->pout->next;
    free(temp);
    q->size--;
    
    if (q->size == 0)
    {
        q->pin = NULL;
        q->pout = NULL;
    }
    return 0;
}

int queue_list_insert_4th(queue_list_t *q, int type, node_t **new_node_ptr)
{
    if (q->size >= LIST_MAX_SIZE) return ERROR_LIST_OVERFLOW;

    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return ERROR_LIST_OVERFLOW;

    new_node->type = type;
    new_node->next = NULL;
    if (new_node_ptr) *new_node_ptr = new_node;

    // ВСЕГДА вставляем на 4-ю позицию от головы (pout — это голова)
    node_t *insert_before = q->pout;

    // Проходим 3 элемента вперёд (0 → 1 → 2 → вставляем перед 3-м индексом)
    for (int i = 0; i < 3 && insert_before != NULL; i++) {
        insert_before = insert_before->next;
    }

    // Если очередь была короткая — insert_before == NULL → вставляем в конец
    if (insert_before == NULL) {
        // очередь < 4 → вставляем в конец
        if (q->pin == NULL) {
            q->pout = q->pin = new_node;
        } else {
            q->pin->next = new_node;
            q->pin = new_node;
        }
    } else {
        // очередь ≥ 4 → вставляем перед 4-м элементом
        new_node->next = insert_before->next;
        insert_before->next = new_node;
        if (new_node->next == NULL) {
            q->pin = new_node;  // если вставили в конец
        }
    }

    q->size++;
    if (q->size > max_size_list) max_size_list = q->size;
    return 0;
}
