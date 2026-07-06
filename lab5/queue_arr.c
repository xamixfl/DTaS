#include "queue_arr.h"

void queue_arr_init(queue_arr_t *q)
{
    q->size = 0;
    q->pin = 0;
    q->pout = 0;
}

int queue_arr_add(queue_arr_t *q, int val)
{
    if ((q->pin >= ARR_MAX_SIZE && q->pout == 0) || q->pin - q->pout >= ARR_MAX_SIZE)
        return ERROR_ARR_OVERFLOW;
    q->data[q->pin] = val;
    q->pin = (q->pin + 1) % ARR_MAX_SIZE; 
    q->size++;
    return 0;
}

int queue_arr_del(queue_arr_t *q, int *val)
{
    if (q->pin == 0)
        return ERROR_EMPTY_ARR;
    *val = q->data[q->pout];
    q->pout = (q->pout + 1) % ARR_MAX_SIZE;
    q->size--;
    return 0;
}

int queue_arr_insert(queue_arr_t *q, int pos, int idx) 
{
    if (q->size >= ARR_MAX_SIZE) 
        return ERROR_ARR_OVERFLOW;

    if (pos > q->size) pos = q->size;
    if (pos < 0) pos = 0;

    int insert_pos = (q->pout + pos) % ARR_MAX_SIZE;

    q->pin = (q->pin + 1) % ARR_MAX_SIZE;

    for (int i = q->size - pos; i > 0; i--) {
        int from = (q->pout + pos + i - 1) % ARR_MAX_SIZE;
        int to   = (from + 1) % ARR_MAX_SIZE;
        q->data[to] = q->data[from];
    }

    q->data[insert_pos] = idx;
    q->size++;
    return 0;
}

int queue_arr_insert_4th(queue_arr_t *q, int val)
{
    if (q->size >= ARR_MAX_SIZE)
        return ERROR_ARR_OVERFLOW;

    int insert_pos = (q->size < 3) ? q->size : 3;

    for (int i = q->size; i > insert_pos; i--) {
        int from = (q->pout + i - 1) % ARR_MAX_SIZE;
        int to = (q->pout + i) % ARR_MAX_SIZE;
        q->data[to] = q->data[from];
    }

    int insert_index = (q->pout + insert_pos) % ARR_MAX_SIZE;
    q->data[insert_index] = val;
    
    q->size++;
    q->pin = (q->pin + 1) % ARR_MAX_SIZE;
    return 0;
}
