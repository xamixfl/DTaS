#ifndef GRAPHVIZ_H
#define GRAPHVIZ_H

#include "bst.h"
#include "avl.h"

#define GV_OK 0
#define GV_ERR_FILE 40

int bst_char_export_to_dot_and_png(bst_node_char_t *root, const char *filename);

int avl_char_export_to_dot_and_png(avl_node_char_t *root, const char *filename);

#endif // GRAPHVIZ_H
