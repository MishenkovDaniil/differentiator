#ifndef TREE_OUTPUT_H
#define TREE_OUTPUT_H

#include "../tree.h"

void tree_tex_print (const Node *left_part, const Node *right_part, FILE *tex_file);
void print_tex_header (FILE *tex_file);
void tex_print_node (const Node *node, FILE *tex_file);
void print_tex_ending (FILE *tex_file);

#endif /* TREE_OUTPUT_H */
