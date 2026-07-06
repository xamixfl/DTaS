#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "hashtable_open.h"

static int is_prime(int n)
{
    if (n <= 1)
        return 0;
    if (n <= 3)
        return 1;
    if (n % 2 == 0 || n % 3 == 0)
        return 0;
    for (int i = 5; i * i <= n; i = i + 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    return 1;
}

static int get_next_prime(int n)
{
    if (n < 2)
        return 2;
    if (is_prime(n))
        return n;
    int p = n + (n % 2 == 0 ? 1 : 2);
    while (!is_prime(p))
        p += 2;
    return p;
}

static int hash_func_char(char key, int size)
{
    int h = (unsigned char)key % size;
    return h;
}

hashtable_open_char_t *hashtable_open_char_create(int size)
{
    int actual_size = size;
    if (size > 2)
    {
        actual_size = get_next_prime(size);
        if (actual_size != size)
        {
            printf("  Автокоррекция: размер %d изменен на простое число %d\n", 
                   size, actual_size);
        }
    }
    
    hashtable_open_char_t *ht = malloc(sizeof(hashtable_open_char_t));
    if (!ht)
        return NULL;

    ht->size = actual_size; 
    ht->count = 0;

    ht->table = malloc(sizeof(ht_entry_char_t) * ht->size);
    if (!ht->table)
    {
        free(ht);
        return NULL;
    }

    for (int i = 0; i < ht->size; i++)
        ht->table[i].status = SLOT_EMPTY;

    return ht;
}

void hashtable_open_char_free(hashtable_open_char_t *ht)
{
    if (ht)
    {
        free(ht->table);
        free(ht);
    }
}

int hashtable_open_char_insert(hashtable_open_char_t *ht, char key)
{
    if (!ht)
        return HT_OPEN_CHAR_ERR_MEM;
  
    if (ht->count == ht->size)
        return HT_OPEN_CHAR_ERR_FULL;
    
    int cmp = 0;
    int found = hashtable_open_char_search(ht, key, &cmp);
    
    if (cmp > MAX_ALLOWED_COMPARISONS)
    {
        hashtable_open_char_rehash(ht, (int)(ht->size * 1.3));
        
        // После реструктуризации нужно заново найти элемент
        cmp = 0;
        found = hashtable_open_char_search(ht, key, &cmp);
    }
    
    // Если элемент найден - увеличить счетчик
    if (found)
    {
        // Нужно найти точный слот и увеличить count
        int idx = hash_func_char(key, ht->size);
        int start = idx;
        int i = 0;
        
        while (ht->table[idx].status != SLOT_EMPTY)
        {
            if (ht->table[idx].status == SLOT_OCCUPIED && 
                ht->table[idx].key == key)
            {
                ht->table[idx].count++;
                return OK;
            }
            idx = (start + ++i) % ht->size;
            if (i >= ht->size)
                break;
        }
        return OK;
    }
    
    // Если элемент не найден - вставить новый
    int idx = hash_func_char(key, ht->size);
    int start = idx;
    int i = 0;

    while (ht->table[idx].status == SLOT_OCCUPIED)
    {
        idx = (start + ++i) % ht->size;
        if (i >= ht->size)
            return HT_OPEN_CHAR_ERR_FULL;
    }

    // Вставка нового элемента
    ht->table[idx].key = key;
    ht->table[idx].count = 1;
    ht->table[idx].status = SLOT_OCCUPIED;
    ht->count++;

    return OK;
}

int hashtable_open_char_search(hashtable_open_char_t *ht, char key, int *comparisons)
{
    if (!ht)
        return 0;

    int idx = hash_func_char(key, ht->size);
    printf("%d\n", idx);
    int start = idx;
    int i = 0;
    *comparisons = 0;

    while (ht->table[idx].status != SLOT_EMPTY)
    {
        (*comparisons)++;
        if (ht->table[idx].status == SLOT_OCCUPIED && ht->table[idx].key == key)
        {
            if (*comparisons > MAX_ALLOWED_COMPARISONS)
                hashtable_open_char_rehash(ht, (int)(ht->size * 1.3));
            return 1;
        }
        idx = (start + ++i) % ht->size;
        if (i >= ht->size)
            break;
    }
    return 0;
}

int hashtable_open_char_delete(hashtable_open_char_t *ht, char key)
{
    if (!ht)
        return 0;

    int idx = hash_func_char(key, ht->size);
    int start = idx;
    int i = 0;

    while (ht->table[idx].status != SLOT_EMPTY)
    {
        if (ht->table[idx].status == SLOT_OCCUPIED && ht->table[idx].key == key)
        {
            if (ht->table[idx].count > 1)
            {
                ht->table[idx].count--;
                return 1;
            }
            ht->table[idx].status = SLOT_DELETED;
            ht->count--;
            return 1;
        }
        idx = (start + ++i) % ht->size;
        if (i >= ht->size)
            break;
    }

    return 0;
}

void hashtable_open_char_print(hashtable_open_char_t *ht)
{
    if (!ht)
        return;

    for (int i = 0; i < ht->size; i++)
    {
        printf("[%d]: ", i);
        if (ht->table[i].status == SLOT_OCCUPIED)
            printf("%c(%d)\n", ht->table[i].key, ht->table[i].count);
        else if (ht->table[i].status == SLOT_DELETED)
            printf("(удалено)\n");
        else
            printf("(пусто)\n");
    }
}

long long hashtable_open_char_memory_usage(hashtable_open_char_t *ht)
{
    if (!ht)
        return 0;
    return sizeof(hashtable_open_char_t) + (ht->size * sizeof(ht_entry_char_t));
}

void hashtable_open_char_rehash(hashtable_open_char_t *ht, int new_size_hint)
{
    if (!ht || new_size_hint <= ht->size)
        return;

    int corrected_size = get_next_prime(new_size_hint);

    hashtable_open_char_t *new_ht = hashtable_open_char_create(corrected_size);
    if (!new_ht)
        return;

    for (int i = 0; i < ht->size; i++)
        if (ht->table[i].status == SLOT_OCCUPIED)
            for (int j = 0; j < ht->table[i].count; j++)
                hashtable_open_char_insert(new_ht, ht->table[i].key);

    free(ht->table);
    ht->table = new_ht->table;
    ht->size = new_ht->size;
    ht->count = new_ht->count;
    free(new_ht);
}

int hashtable_open_char_max_collisions(hashtable_open_char_t *ht)
{
    if (!ht)
        return 0;
    int max = 0;
    for (int i = 0; i < ht->size; i++)
    {
        int probe = 0;
        int idx = i;
        while (ht->table[idx].status != SLOT_EMPTY)
        {
            probe++;
            if (++idx >= ht->size)
                idx = 0;
            if (probe >= ht->size)
                break;
        }
        if (probe > max)
            max = probe;
    }
    return max;
}

int hashtable_open_char_max_comparisons(hashtable_open_char_t *ht)
{
    if (!ht || ht->count == 0)
        return 0;
    int max_cmp = 0;
    for (int j = 0; j < ht->size; j++)
    {
        if (ht->table[j].status == SLOT_OCCUPIED)
        {
            char key = ht->table[j].key;
            int cmp = 0;
            hashtable_open_char_search(ht, key, &cmp);
            if (cmp > max_cmp)
                max_cmp = cmp;
        }
    }
    return max_cmp;
}

void hashtable_open_char_build_from_string(hashtable_open_char_t *ht, const char *str)
{
    if (!ht || !str)
        return;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (isalpha(str[i]))
        {
            hashtable_open_char_insert(ht, str[i]);
        }
    }
}
