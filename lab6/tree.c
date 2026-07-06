#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

#include <stdarg.h>

void print_red(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    printf(COLOR_RED);
    vprintf(fmt, args);
    printf(COLOR_RESET);
    va_end(args);
}

void print_green(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf(COLOR_GREEN);
    vprintf(fmt, args);
    printf(COLOR_RESET);
    va_end(args);
}

void print_yellow(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf(COLOR_YELLOW);
    vprintf(fmt, args);
    printf(COLOR_RESET);
    va_end(args);
}

void print_cyan(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf(COLOR_CYAN);
    vprintf(fmt, args);
    printf(COLOR_RESET);
    va_end(args);
}

tree_node_t* tree_create_node(char data) 
{
    tree_node_t *node = malloc(sizeof(tree_node_t));
    if (node) {
        node->data = data;
        node->left = node->right = NULL;
    }
    return node;
}

tree_node_t* tree_insert(tree_node_t *root, char data) 
{
    if (root == NULL)
        return tree_create_node(data);

    if (data < root->data)
        root->left = tree_insert(root->left, data);
    else
        root->right = tree_insert(root->right, data);

    return root;
}

void count_frequency(tree_node_t *root, int freq[256]) 
{
    if (!root) 
        return;
    freq[(unsigned char)root->data]++;
    count_frequency(root->left, freq);
    count_frequency(root->right, freq);
}

void tree_print_colored(tree_node_t *root, int level, const int freq[256]) 
{
    if (!root) 
        return;
    tree_print_colored(root->right, level + 1, freq);
    for (int i = 0; i < level * 4; i++) 
        printf(" ");

    if (freq[(unsigned char)root->data] > 1)
        printf(COLOR_RED "%c" COLOR_RESET "\n", root->data);
    else
        printf(COLOR_GREEN "%c" COLOR_RESET "\n", root->data);

    tree_print_colored(root->left, level + 1, freq);
}

static tree_node_t* find_min(tree_node_t* node) 
{
    while (node && node->left) 
        node = node->left;
    return node;
}

tree_node_t* tree_delete(tree_node_t *root, char data) 
{
    if (!root) 
        return NULL;
    if (data < root->data)
        root->left = tree_delete(root->left, data);
    else if (data > root->data)
        root->right = tree_delete(root->right, data);
    else 
    {
        if (!root->left) 
        { 
            tree_node_t *t = root->right; free(root); return t;
        }
        if (!root->right) 
        { 
            tree_node_t *t = root->left; free(root); return t; 
        }
        tree_node_t *temp = find_min(root->right);
        root->data = temp->data;
        root->right = tree_delete(root->right, temp->data);
    }
    return root;
}

tree_node_t* tree_search(tree_node_t *root, char data) 
{
    if (!root || root->data == data) 
        return root;
    if (data < root->data) 
        return tree_search(root->left, data);
    return tree_search(root->right, data);
}

tree_node_t* remove_duplicates_from_tree(tree_node_t *root) 
{
    if (!root) 
        return NULL;
    int freq[256] = {0};
    count_frequency(root, freq);
    for (int i = 0; i < 256; i++) 
    {
        if (freq[i] > 1) 
        {
            while (tree_search(root, (char)i))
                root = tree_delete(root, (char)i);
        }
    }
    return root;
}

void tree_traverse_postorder(tree_node_t *root) 
{
    if (!root) 
        return;
    tree_traverse_postorder(root->left);
    tree_traverse_postorder(root->right);
    printf("%c ", root->data);
}

void tree_free(tree_node_t *root) 
{
    if (!root) 
        return;
    tree_free(root->left);
    tree_free(root->right);
    free(root);
}

char* remove_duplicates_from_string(const char *str) 
{
    int freq[256] = {0};
    int len = strlen(str);
    char *res = malloc(len + 1);
    
    int pos = 0;
    for (int i = 0; i < len; i++) 
        freq[(unsigned char)str[i]]++;
    for (int i = 0; i < len; i++)
    {
        if (freq[(unsigned char)str[i]] == 1)
            res[pos++] = str[i];
    }

    res[pos] = '\0';
    return res;
}

// === Префиксный обход (корень - лево - право) ===
void tree_traverse_preorder(tree_node_t *root) 
{
    if (!root) return;
    printf("%c ", root->data);
    tree_traverse_preorder(root->left);
    tree_traverse_preorder(root->right);
}

// === Инфиксный обход (лево - корень - право) ===
void tree_traverse_inorder(tree_node_t *root) 
{
    if (!root) return;
    tree_traverse_inorder(root->left);
    printf("%c ", root->data);
    tree_traverse_inorder(root->right);
}
