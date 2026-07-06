#ifndef AVL_H
#define AVL_H

typedef struct avl_node_char
{
    char data;               // Буква. 
    int count;               // Количество вхождений. 
    int height;              // Высота поддерева с корнем в данном узле. 
    struct avl_node_char *left;  // Указатель на левое поддерево. 
    struct avl_node_char *right; // Указатель на правое поддерево. 
} avl_node_char_t;

avl_node_char_t *avl_char_insert(avl_node_char_t *root, char data);

avl_node_char_t *avl_char_delete(avl_node_char_t *root, char data, int *success);

avl_node_char_t *avl_char_search(avl_node_char_t *root, char data, int *comparisons);

void avl_char_print(avl_node_char_t *root);

void avl_char_free(avl_node_char_t *root);

void avl_char_count_levels(avl_node_char_t *root, int *counts, int level, int *max_depth, int max_limit);

long long avl_char_memory_usage(avl_node_char_t *root);

int avl_char_get_height(avl_node_char_t *root);

void avl_char_build_from_string(avl_node_char_t **root, const char *str);

#endif // AVL_H
