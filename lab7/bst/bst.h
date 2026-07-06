#ifndef BST_H
#define BST_H

typedef struct bst_node_char
{
    char data;               // Буква. 
    int count;               // Количество вхождений.
    struct bst_node_char *left;  // Указатель на левое поддерево.
    struct bst_node_char *right; // Указатель на правое поддерево.
} bst_node_char_t;

bst_node_char_t* bst_char_create_node(char data);

bst_node_char_t* bst_char_insert(bst_node_char_t *root, char data);

bst_node_char_t* bst_char_search(bst_node_char_t *root, char data, int *comparisons);

bst_node_char_t* bst_char_delete(bst_node_char_t *root, char data, int *success);

void bst_char_print(bst_node_char_t *root);

void bst_char_free(bst_node_char_t *root);

void bst_char_count_levels(bst_node_char_t *root, int *counts, int level, int *max_depth, int max_limit);

long long bst_char_memory_usage(bst_node_char_t *root);

int bst_char_get_height(bst_node_char_t *root);

void bst_char_build_from_string(bst_node_char_t **root, const char *str);

#endif // BST_H
