#ifndef TREE_H
#define TREE_H

#define ERR_MEM_ALLOC     10
#define ERR_EMPTY_TREE    12
#define ERR_NOT_FOUND     13
#define ERR_INVALID_INPUT 1

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

typedef struct tree_node 
{
    char data;
    struct tree_node *left;
    struct tree_node *right;
} tree_node_t;

void print_red(const char *fmt, ...);
void print_green(const char *fmt, ...);
void print_yellow(const char *fmt, ...);
void print_cyan(const char *fmt, ...);

tree_node_t* tree_create_node(char data);
tree_node_t* tree_insert(tree_node_t *root, char data);         
tree_node_t* tree_search(tree_node_t *root, char data);
tree_node_t* tree_delete(tree_node_t *root, char data);
void count_frequency(tree_node_t *root, int freq[256]);
void tree_print_colored(tree_node_t *root, int level, const int freq[256]);
tree_node_t* remove_duplicates_from_tree(tree_node_t *root);
void tree_traverse_postorder(tree_node_t *root);
void tree_free(tree_node_t *root);
char* remove_duplicates_from_string(const char *str);
void tree_traverse_preorder(tree_node_t *root);
void tree_traverse_inorder(tree_node_t *root);

#endif
