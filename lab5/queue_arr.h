#ifndef _QUEUE_ARR_H_
#define _QUEUE_ARR_H_

#define ARR_MAX_SIZE 1000
#define ERROR_ARR_OVERFLOW -1
#define ERROR_EMPTY_ARR -2

typedef struct
{
    int data[ARR_MAX_SIZE];
    int pin;
    int pout;
    int size;
} queue_arr_t;

void queue_arr_init(queue_arr_t *q);
int queue_arr_add(queue_arr_t *q, int val);
int queue_arr_del(queue_arr_t *q, int *val);
int queue_arr_insert(queue_arr_t *q, int pos, int idx);
int queue_arr_insert_4th(queue_arr_t *q, int val);


#endif
