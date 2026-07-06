#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphviz.h"

static void bst_char_nodes_to_dot_avl_style(bst_node_char_t *node, FILE *f)
{
    if (!node)
        return;

    fprintf(f, "    node%p [label=\"%c\\n(%d)\", shape=record, style=filled, fillcolor=lightgrey];\n", 
            (void*)node, node->data, node->count);

    if (node->left)
    {
        fprintf(f, "    node%p -> node%p [color=blue];\n", (void*)node, (void*)node->left);
        bst_char_nodes_to_dot_avl_style(node->left, f);
    }
    if (node->right)
    {
        fprintf(f, "    node%p -> node%p [color=red];\n", (void*)node, (void*)node->right);
        bst_char_nodes_to_dot_avl_style(node->right, f);
    }
}

int bst_char_export_to_dot_and_png(bst_node_char_t *root, const char *filename)
{
    char dot_filename[256];
    char png_filename[256];
    char command[1024];

    snprintf(dot_filename, sizeof(dot_filename), "%s.dot", filename);
    snprintf(png_filename, sizeof(png_filename), "%s.png", filename);

    FILE *f = fopen(dot_filename, "w");
    if (!f)
        return GV_ERR_FILE;

    fprintf(f, "digraph BST {\n");
    fprintf(f, "    node [fontname=\"Arial\"];\n");
    
    if (!root)
        fprintf(f, "    start [label=\"Empty Tree\", shape=plaintext];\n");
    else
        bst_char_nodes_to_dot_avl_style(root, f);

    fprintf(f, "}\n");
    fclose(f);

    snprintf(command, sizeof(command), "dot -Tpng %s -o %s", dot_filename, png_filename);
    
    printf("Генерация графа BST: %s\n", command);
    int rc = system(command);

    if (rc == 0)
        printf("Файл '%s' успешно создан.\n", png_filename);
    else
        printf("Ошибка при запуске Graphviz. Убедитесь, что 'dot' установлен и доступен в PATH.\n");

    return GV_OK;
}

static void avl_char_nodes_to_dot(avl_node_char_t *node, FILE *f)
{
    if (!node)
        return;

    fprintf(f, "    node%p [label=\"%c\\n(%d) | h=%d\", shape=record];\n", 
            (void*)node, node->data, node->count, node->height);

    if (node->left)
    {
        fprintf(f, "    node%p -> node%p [color=blue];\n", (void*)node, (void*)node->left);
        avl_char_nodes_to_dot(node->left, f);
    }
    if (node->right)
    {
        fprintf(f, "    node%p -> node%p [color=red];\n", (void*)node, (void*)node->right);
        avl_char_nodes_to_dot(node->right, f);
    }
}

int avl_char_export_to_dot_and_png(avl_node_char_t *root, const char *filename)
{
    char dot_filename[256];
    char png_filename[256];
    char command[1024];

    snprintf(dot_filename, sizeof(dot_filename), "%s.dot", filename);
    snprintf(png_filename, sizeof(png_filename), "%s.png", filename);

    FILE *f = fopen(dot_filename, "w");
    if (!f)
        return GV_ERR_FILE;

    fprintf(f, "digraph AVL {\n");
    fprintf(f, "    node [fontname=\"Arial\", style=filled, fillcolor=lightgrey];\n");

    if (!root)
        fprintf(f, "    start [label=\"Empty Tree\", shape=plaintext];\n");
    else
        avl_char_nodes_to_dot(root, f);

    fprintf(f, "}\n");
    fclose(f);

    snprintf(command, sizeof(command), "dot -Tpng %s -o %s", dot_filename, png_filename);
    
    printf("Генерация графа AVL: %s\n", command);
    int rc = system(command);

    if (rc == 0)
        printf("Файл '%s' успешно создан.\n", png_filename);
    else
        printf("Ошибка при запуске Graphviz.\n");

    return GV_OK;
}
