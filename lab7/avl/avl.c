#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "avl.h"

static int get_height(avl_node_char_t *n)
{
    return n ? n->height : 0;
}

static int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

static int get_balance(avl_node_char_t *n)
{
    return n ? get_height(n->left) - get_height(n->right) : 0;
}

static avl_node_char_t *avl_char_create_node(char data)
{
    avl_node_char_t *node = malloc(sizeof(avl_node_char_t));
    if (node)
    {
        node->data = data;
        node->count = 1;
        node->height = 1;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

static avl_node_char_t *right_rotate(avl_node_char_t *y)
{
    avl_node_char_t *x = y->left;
    avl_node_char_t *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max_int(get_height(y->left), get_height(y->right)) + 1;
    x->height = max_int(get_height(x->left), get_height(x->right)) + 1;
    return x;
}

static avl_node_char_t *left_rotate(avl_node_char_t *x)
{
    avl_node_char_t *y = x->right;
    avl_node_char_t *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max_int(get_height(x->left), get_height(x->right)) + 1;
    y->height = max_int(get_height(y->left), get_height(y->right)) + 1;
    return y;
}

avl_node_char_t *avl_char_insert(avl_node_char_t *node, char data)
{
    if (node == NULL)
        return avl_char_create_node(data);

    if (data < node->data)
        node->left = avl_char_insert(node->left, data);
    else if (data > node->data)
        node->right = avl_char_insert(node->right, data);
    else
    {
        node->count++;
        return node;
    }

    node->height = 1 + max_int(get_height(node->left), get_height(node->right));

    int balance = get_balance(node);

    if (balance > 1 && data < node->left->data)
        return right_rotate(node);

    if (balance < -1 && data > node->right->data)
        return left_rotate(node);

    if (balance > 1 && data > node->left->data)
    {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    if (balance < -1 && data < node->right->data)
    {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

static avl_node_char_t *min_value_node(avl_node_char_t *node)
{
    avl_node_char_t *current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

avl_node_char_t *avl_char_delete(avl_node_char_t *root, char data, int *success)
{
    if (root == NULL)
    {
        if (success)
            *success = 0;
        return root;
    }

    if (data < root->data)
        root->left = avl_char_delete(root->left, data, success);
    else if (data > root->data)
        root->right = avl_char_delete(root->right, data, success);
    else
    {
        if (success)
            *success = 1;

        if (root->count > 1)
        {
            root->count--;
            return root;
        }

        if ((root->left == NULL) || (root->right == NULL))
        {
            avl_node_char_t *temp = root->left ? root->left : root->right;

            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else
                *root = *temp;

            free(temp);
        }
        else
        {
            avl_node_char_t *temp = min_value_node(root->right);
            root->data = temp->data;
            root->count = temp->count;
            root->right = avl_char_delete(root->right, temp->data, success);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max_int(get_height(root->left), get_height(root->right));

    int balance = get_balance(root);

    if (balance > 1 && get_balance(root->left) >= 0)
        return right_rotate(root);

    if (balance > 1 && get_balance(root->left) < 0)
    {
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }

    if (balance < -1 && get_balance(root->right) <= 0)
        return left_rotate(root);

    if (balance < -1 && get_balance(root->right) > 0)
    {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    return root;
}

avl_node_char_t *avl_char_search(avl_node_char_t *root, char data, int *comparisons)
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
        return avl_char_search(root->left, data, comparisons);
    return avl_char_search(root->right, data, comparisons);
}

static void avl_char_print_visual(avl_node_char_t *root, const char *prefix, int is_left, int is_only_child)
{
    if (root == NULL)
        return;

    printf("%s", prefix);

    if (is_only_child)
        printf("└── ");
    else
        printf(is_left ? "├── " : "└── ");
        
    printf("%c(%d)\n", root->data, root->count);

    char new_prefix[256];
    
    if (is_only_child)
        snprintf(new_prefix, sizeof(new_prefix), "%s    ", prefix);
    else
        snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_left ? "│   " : "    ");

    int left_is_only = (root->right == NULL);
    int right_is_only = 1;
    
    avl_char_print_visual(root->left, new_prefix, 1, left_is_only);
    avl_char_print_visual(root->right, new_prefix, 0, right_is_only);
}

void avl_char_print(avl_node_char_t *root)
{
    if (root == NULL)
    {
        printf("Дерево пусто.\n");
        return;
    }

    printf("%c(%d)\n", root->data, root->count);

    int left_has_sibling = (root->right != NULL);
    avl_char_print_visual(root->left, "", 1, !left_has_sibling);
    avl_char_print_visual(root->right, "", 0, 1);
}

void avl_char_free(avl_node_char_t *root)
{
    if (root == NULL)
        return;
    avl_char_free(root->left);
    avl_char_free(root->right);
    free(root);
}

void avl_char_count_levels(avl_node_char_t *root, int *counts, int level, int *max_depth, int max_limit)
{
    if (root == NULL)
        return;

    if (level < max_limit)
    {
        counts[level]++;
        if (level > *max_depth)
            *max_depth = level;
    }

    avl_char_count_levels(root->left, counts, level + 1, max_depth, max_limit);
    avl_char_count_levels(root->right, counts, level + 1, max_depth, max_limit);
}

long long avl_char_memory_usage(avl_node_char_t *root)
{
    if (root == NULL)
        return 0;
    return sizeof(avl_node_char_t) + avl_char_memory_usage(root->left) + avl_char_memory_usage(root->right);
}

int avl_char_get_height(avl_node_char_t *root)
{
    if (!root)
        return 0;
    int left_h = avl_char_get_height(root->left);
    int right_h = avl_char_get_height(root->right);
    return (left_h > right_h ? left_h : right_h) + 1;
}

void avl_char_build_from_string(avl_node_char_t **root, const char *str)
{
    if (!str || !root)
        return;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (isalpha(str[i]))
        {
            *root = avl_char_insert(*root, str[i]);
        }
    }
}
