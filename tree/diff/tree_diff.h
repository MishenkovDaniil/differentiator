#ifndef TREE_DIFF_H
#define TREE_DIFF_H

#include "../tree.h"

Node *tree_diff (const Node *node, FILE *tex_file, unsigned int *err = &DEF_ERRNO);
Node *cpy_node (const Node *node);

#endif /* TREE_DIFF_H */
