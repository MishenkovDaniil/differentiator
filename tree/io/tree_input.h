#ifndef TREE_INPUT_H
#define TREE_INPUT_H

#include "../tree.h"

void skip_spaces (const char **str);

Node *tree_fill (Tree *tree, FILE *input_file, const char *file_name);

Node *GetNodeG (Tree *tree, const char *expr);
Node *GetNodeE (const char **expr);
Node *GetNodeT (const char **expr);
Node *GetNodeP (const char **expr);
Node *GetNodeN (const char **expr);
Node *GetNodeV (const char **expr);

#endif /* TREE_INPUT_H */
