#ifndef TREE_OUTPUT_H
#define TREE_OUTPUT_H

#include "../tree.h"

int max (int val_1, int val_2);
void tree_tex_print (const Node *left_part, const Node *right_part, FILE *tex_file, bool is_diff = false, int diff_order = 1);
void print_tex_header (FILE *tex_file);
void tex_print_node (const Node *node, FILE *tex_file);
void print_tex_ending (FILE *tex_file);
void new_section (FILE *tex_file);
void print_original_function (const char *func_name, Tree *func, FILE *tex_file);
void print_function (const char *func_name, Tree *func, FILE *tex_file);
void print_diff_func (const char *func_name, Tree *diff_func, FILE *tex_file, int diff_order);
void print_point_value (const char *func_name, double point, double point_value, FILE *tex_file);
void print_decompose_tree (const char *func_name, Tree *decompose_tree, double decompose_point, int decompose_order,
                           FILE *tex_file);

#endif /* TREE_OUTPUT_H */
