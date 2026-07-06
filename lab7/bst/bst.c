#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "bst.h"

bst_node_char_t* bst_char_create_node(char data)
{
    bst_node_char_t *node = malloc(sizeof(bst_node_char_t));
    if (node)
    {
        node->data = data;
        node->count = 1;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

bst_node_char_t* bst_char_insert(bst_node_char_t *root, char data)
{
    if (root == NULL)
        return bst_char_create_node(data);

    if (data < root->data)
        root->left = bst_char_insert(root->left, data);
    else if (data > root->data)
        root->right = bst_char_insert(root->right, data);
    else
        root->count++;

    return root;
}

bst_node_char_t* bst_char_search(bst_node_char_t *root, char data, int *comparisons)
{
    if (root == NULL)
    {
        if (comparisons)
            (*comparisons)++;
        return NULL;
    }

    if (comparisons)
        (*comparisons)++;
    if (data == root->data)
        return root;
    if (data < root->data)
        return bst_char_search(root->left, data, comparisons);
    return bst_char_search(root->right, data, comparisons);
}

static bst_node_char_t* remove_min_node(bst_node_char_t* root, bst_node_char_t** min_out)
{
    if (root->left == NULL)
    {
        *min_out = root;
        return root->right;
    }
    root->left = remove_min_node(root->left, min_out);
    return root;
}

bst_node_char_t* bst_char_delete(bst_node_char_t *root, char data, int *success)
{
    if (root == NULL)
    {
        if (success)
            *success = 0;
        return NULL;
    }

    if (data < root->data)
        root->left = bst_char_delete(root->left, data, success);
    else if (data > root->data)
        root->right = bst_char_delete(root->right, data, success);
    else
    {
        if (success)
            *success = 1;

        if (root->count > 1)
        {
            root->count--;
            return root;
        }

        bst_node_char_t *temp;

        if (root->left == NULL)
        {
            temp = root->right;
            free(root);
            return temp;
        }
        if (root->right == NULL)
        {
            temp = root->left;
            free(root);
            return temp;
        }
        
        bst_node_char_t *min_node;
        bst_node_char_t *new_right = remove_min_node(root->right, &min_node);

        min_node->left = root->left;
        min_node->right = new_right;

        free(root);
        return min_node;
    }
    return root;
}

static void bst_char_print_visual(bst_node_char_t *root, const char *prefix, int is_left, int is_only_child)
{
    if (root == NULL)
        return;

    printf("%s", prefix);

    // Если у родителя только один потомок (левый), то это "└──"
    if (is_only_child)
        printf("└── ");
    else
        printf(is_left ? "├── " : "└── ");
        
    printf("%c(%d)\n", root->data, root->count);

    char new_prefix[256];
    
    // Добавляем отступы в зависимости от того, последний ли это потомок
    if (is_only_child)
        snprintf(new_prefix, sizeof(new_prefix), "%s    ", prefix);
    else
        snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_left ? "│   " : "    ");

    // Проверяем, является ли текущий узел родителем с одним потомком
    int left_is_only = (root->right == NULL);
    int right_is_only = 1; // Правый всегда последний если он есть
    
    bst_char_print_visual(root->left, new_prefix, 1, left_is_only);
    bst_char_print_visual(root->right, new_prefix, 0, right_is_only);
}

void bst_char_print(bst_node_char_t *root)
{
    if (root == NULL)
    {
        printf("Дерево пусто.\n");
        return;
    }

    printf("%c(%d)\n", root->data, root->count);

    // Проверяем, есть ли у корня оба потомка
    int left_has_sibling = (root->right != NULL);
    bst_char_print_visual(root->left, "", 1, !left_has_sibling);
    bst_char_print_visual(root->right, "", 0, 1); // Правый всегда "последний"
}

void bst_char_free(bst_node_char_t *root)
{
    if (root == NULL)
        return;
    bst_char_free(root->left);
    bst_char_free(root->right);
    free(root);
}

void bst_char_count_levels(bst_node_char_t *root, int *counts, int level, int *max_depth, int max_limit)
{
    if (root == NULL)
        return;

    if (level < max_limit)
    {
        counts[level]++;
        if (level > *max_depth)
            *max_depth = level;
    }

    bst_char_count_levels(root->left, counts, level + 1, max_depth, max_limit);
    bst_char_count_levels(root->right, counts, level + 1, max_depth, max_limit);
}

long long bst_char_memory_usage(bst_node_char_t *root)
{
    if (root == NULL)
        return 0;
    return sizeof(bst_node_char_t) + bst_char_memory_usage(root->left) + bst_char_memory_usage(root->right);
}

int bst_char_get_height(bst_node_char_t *root)
{
    if (!root)
        return 0;
    int left_h = bst_char_get_height(root->left);
    int right_h = bst_char_get_height(root->right);
    return (left_h > right_h ? left_h : right_h) + 1;
}

void bst_char_build_from_string(bst_node_char_t **root, const char *str)
{
    if (!str || !root)
        return;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (isalpha(str[i]))
        {
            *root = bst_char_insert(*root, str[i]);
        }
    }
}
