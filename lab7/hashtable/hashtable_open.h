#ifndef HASHTABLE_OPEN_H
#define HASHTABLE_OPEN_H

#define MAX_ALLOWED_COMPARISONS 4

#define OK 0
#define HT_OPEN_CHAR_ERR_MEM 10
#define HT_OPEN_CHAR_ERR_FULL 11

typedef enum
{
    SLOT_EMPTY,    // Слот свободен.
    SLOT_OCCUPIED, // Слот занят элементом. 
    SLOT_DELETED   // Слот содержит удалённый элемент. 
} slot_status_t;

typedef struct
{
    char key;               // Буква. 
    int count;              // Количество вхождений. 
    slot_status_t status;   // Текущее состояние слота. 
} ht_entry_char_t;

typedef struct
{
    ht_entry_char_t *table; // Указатель на массив слотов.
    int size;               // Размер хеш-таблицы.
    int count;              // Количество занятых элементов.
} hashtable_open_char_t;

hashtable_open_char_t *hashtable_open_char_create(int size);

void hashtable_open_char_free(hashtable_open_char_t *ht);

int hashtable_open_char_insert(hashtable_open_char_t *ht, char key);

int hashtable_open_char_search(hashtable_open_char_t *ht, char key, int *comparisons);

int hashtable_open_char_delete(hashtable_open_char_t *ht, char key);

void hashtable_open_char_print(hashtable_open_char_t *ht);

long long hashtable_open_char_memory_usage(hashtable_open_char_t *ht);

void hashtable_open_char_rehash(hashtable_open_char_t *ht, int new_size);

int hashtable_open_char_max_collisions(hashtable_open_char_t *ht);

int hashtable_open_char_max_comparisons(hashtable_open_char_t *ht);

void hashtable_open_char_build_from_string(hashtable_open_char_t *ht, const char *str);

#endif // HASHTABLE_OPEN_H
