#ifndef TREE_INPUT_H
#define TREE_INPUT_H

#include "../tree.h"
#include "../../config.h"

//bool get_config (Config *config);
bool get_config (Tree *expr, Config *config, FILE *input_file, const char *input_file_name);
int get_file_size (const char *file_name);               ///repo onegin ../io/io.cpp

void skip_spaces (const char **str);

//Node *tree_fill (Tree *tree, FILE *input_file, const char *file_name);
Node *tree_fill (Tree *tree, Config *config);

Node *GetNodeG (Tree *tree, const char *expr);
Node *GetNodeE (const char **expr);
Node *GetNodeT (const char **expr);
Node *GetNodeD (const char **expr);
Node *GetNodeP (const char **expr);
Node *GetNodeN (const char **expr);
Node *GetNodeV (const char **expr);

#endif /* TREE_INPUT_H */
