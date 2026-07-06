#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "hashtable_closed.h"

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

hashtable_closed_char_t *hashtable_closed_char_create(int size)
{
    // Ищем ближайшее простое число >= size
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
    
    hashtable_closed_char_t *ht = malloc(sizeof(hashtable_closed_char_t));
    if (!ht)
        return NULL;

    ht->size = actual_size;
    ht->count = 0;

    ht->buckets = malloc(sizeof(ht_node_char_t *) * ht->size);
    if (!ht->buckets)
    {
        free(ht);
        return NULL;
    }

    for (int i = 0; i < ht->size; i++)
        ht->buckets[i] = NULL;

    return ht;
}

void hashtable_closed_char_free(hashtable_closed_char_t *ht)
{
    if (ht)
    {
        for (int i = 0; i < ht->size; i++)
        {
            ht_node_char_t *current = ht->buckets[i];
            while (current)
            {
                ht_node_char_t *temp = current;
                current = current->next;
                free(temp);
            }
        }
        free(ht->buckets);
        free(ht);
    }
}

int hashtable_closed_char_insert(hashtable_closed_char_t *ht, char key)
{
    if (!ht)
        return HT_CLOSED_CHAR_ERR_MEM;

    int cmp = 0;
    if (hashtable_closed_char_search(ht, key, &cmp))
    {
        int idx = hash_func_char(key, ht->size);
        ht_node_char_t *cur = ht->buckets[idx];
        while (cur)
        {
            if (cur->key == key)
            {
                cur->count++;
                return OK;
            }
            cur = cur->next;
        }
        return OK;
    }

    int idx = hash_func_char(key, ht->size);

    ht_node_char_t *new_node = malloc(sizeof(ht_node_char_t));
    if (!new_node)
        return HT_CLOSED_CHAR_ERR_MEM;

    new_node->key = key;
    new_node->count = 1;
    new_node->next = ht->buckets[idx];
    ht->buckets[idx] = new_node;
    ht->count++;

    if (cmp > MAX_ALLOWED_COMPARISONS)
        hashtable_closed_char_rehash(ht, (int)(ht->size * 1.3));

    return OK;
}

int hashtable_closed_char_search(hashtable_closed_char_t *ht, char key, int *comparisons)
{
    if (!ht)
        return 0;

    int idx = hash_func_char(key, ht->size);
    ht_node_char_t *cur = ht->buckets[idx];
    *comparisons = 0;

    while (cur)
    {
        (*comparisons)++;
        if (cur->key == key)
        {
            if (*comparisons > MAX_ALLOWED_COMPARISONS)
                hashtable_closed_char_rehash(ht, (int)(ht->size * 1.3));
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

int hashtable_closed_char_delete(hashtable_closed_char_t *ht, char key)
{
    if (!ht)
        return 0;

    int idx = hash_func_char(key, ht->size);
    ht_node_char_t *current = ht->buckets[idx];
    ht_node_char_t *prev = NULL;

    while (current)
    {
        if (current->key == key)
        {
            if (current->count > 1)
            {
                current->count--;
                return 1;
            }
            if (prev)
                prev->next = current->next;
            else
                ht->buckets[idx] = current->next;
            free(current);
            ht->count--;
            return 1;
        }
        prev = current;
        current = current->next;
    }

    return 0;
}

void hashtable_closed_char_print(hashtable_closed_char_t *ht)
{
    if (!ht)
        return;

    for (int i = 0; i < ht->size; i++)
    {
        printf("[%d]: ", i);
        ht_node_char_t *current = ht->buckets[i];
        while (current)
        {
            printf("%c(%d) -> ", current->key, current->count);
            current = current->next;
        }
        printf("NULL\n");
    }
}

long long hashtable_closed_char_memory_usage(hashtable_closed_char_t *ht)
{
    if (!ht)
        return 0;
    long long mem = sizeof(hashtable_closed_char_t) + (ht->size * sizeof(ht_node_char_t *));
    for (int i = 0; i < ht->size; i++)
    {
        ht_node_char_t *current = ht->buckets[i];
        while (current)
        {
            mem += sizeof(ht_node_char_t);
            current = current->next;
        }
    }
    return mem;
}

void hashtable_closed_char_rehash(hashtable_closed_char_t *ht, int new_size_hint)
{
    if (!ht || new_size_hint <= ht->size)
        return;

    int corrected_size = get_next_prime(new_size_hint);

    hashtable_closed_char_t *new_ht = hashtable_closed_char_create(corrected_size);
    if (!new_ht)
        return;

    for (int i = 0; i < ht->size; i++)
    {
        ht_node_char_t *cur = ht->buckets[i];
        while (cur)
        {
            for (int j = 0; j < cur->count; j++)
                hashtable_closed_char_insert(new_ht, cur->key);
            cur = cur->next;
        }
    }

    for (int i = 0; i < ht->size; i++)
    {
        ht_node_char_t *tmp = ht->buckets[i];
        while (tmp)
        {
            ht_node_char_t *next = tmp->next;
            free(tmp);
            tmp = next;
        }
    }
    free(ht->buckets);

    ht->buckets = new_ht->buckets;
    ht->size = new_ht->size;
    ht->count = new_ht->count;
    free(new_ht);
}

int hashtable_closed_char_max_collisions(hashtable_closed_char_t *ht)
{
    if (!ht)
        return 0;
    int max = 0;
    for (int i = 0; i < ht->size; i++)
    {
        int len = 0;
        ht_node_char_t *cur = ht->buckets[i];
        while (cur)
        {
            len++;
            cur = cur->next;
        }
        if (len > max)
            max = len;
    }
    return max;
}

int hashtable_closed_char_max_comparisons(hashtable_closed_char_t *ht)
{
    if (!ht || ht->count == 0)
        return 0;
    int max_cmp = 0;
    for (int j = 0; j < ht->size; j++)
    {
        ht_node_char_t *cur = ht->buckets[j];
        while (cur)
        {
            char key = cur->key;
            int cmp = 0;
            hashtable_closed_char_search(ht, key, &cmp);
            if (cmp > max_cmp)
                max_cmp = cmp;
            cur = cur->next;
        }
    }
    return max_cmp;
}

void hashtable_closed_char_build_from_string(hashtable_closed_char_t *ht, const char *str)
{
    if (!ht || !str)
        return;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (isalpha(str[i]))
        {
            hashtable_closed_char_insert(ht, str[i]);
        }
    }
}
