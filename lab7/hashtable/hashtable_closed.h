#ifndef HASHTABLE_CLOSED_H
#define HASHTABLE_CLOSED_H

#define MAX_ALLOWED_COMPARISONS 4

#define OK 0
#define HT_CLOSED_CHAR_ERR_MEM 20

typedef struct ht_node_char
{
    char key;              // Буква.
    int count;             // Количество вхождений.
    struct ht_node_char *next; // Указатель на следующий узел в цепочке.
} ht_node_char_t;

typedef struct
{
    ht_node_char_t **buckets; // Массив указателей на головы списков.
    int size;            // Количество бакетов.
    int count;           // Общее количество элементов в таблице.
} hashtable_closed_char_t;

hashtable_closed_char_t *hashtable_closed_char_create(int size);

void hashtable_closed_char_free(hashtable_closed_char_t *ht);

int hashtable_closed_char_insert(hashtable_closed_char_t *ht, char key);

int hashtable_closed_char_search(hashtable_closed_char_t *ht, char key, int *comparisons);

int hashtable_closed_char_delete(hashtable_closed_char_t *ht, char key);

void hashtable_closed_char_print(hashtable_closed_char_t *ht);

long long hashtable_closed_char_memory_usage(hashtable_closed_char_t *ht);

void hashtable_closed_char_rehash(hashtable_closed_char_t *ht, int new_size);

int hashtable_closed_char_max_collisions(hashtable_closed_char_t *ht);

int hashtable_closed_char_max_comparisons(hashtable_closed_char_t *ht);

void hashtable_closed_char_build_from_string(hashtable_closed_char_t *ht, const char *str);

#endif // HASHTABLE_CLOSED_H
