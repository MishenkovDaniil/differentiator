#ifndef TREE_OUTPUT_H
#define TREE_OUTPUT_H

#include "../tree.h"
#include "../../config.h"

#define debug_print(...)                                                                            \
do                                                                                                  \
{                                                                                                   \
    printf (__VA_ARGS__);                                                                           \
    fprintf (stderr, ", func %s in file %s, line %d.\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);  \
}while (0)

#define tex_print(...) fprintf (tex_file, __VA_ARGS__)

void tex_print_func (const char *func_name, Tree *expr, FILE *tex_file);
void tex_find_point_value (Tree *expr, const char *func_name, Config *config, FILE *tex_file);
void tex_find_diff (Tree *expr, const char *func_name, Config *config, FILE *tex_file);
void tex_decompose_func (Tree *expr, const char *func_name, const char *graphic_file_name, const char *input_file_name,
                         Config *config, FILE *tex_file, FILE *graphic_file);


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
void tex_print_graphic (FILE *graphic_file, const char *graphic_file_name, const char *input_file_name,
                        Tree *func, FILE *tex_file);
bool func_for_gnuplot (Node *func_node, char *graphic_func);



void tex_print_compressed_node (const Node *node, FILE *tex_file, const Node *original_node);
void tree_tex_print_compressed (const Node *left_part, const Node *right_part, const Node *original_right_part,
                                FILE *tex_file, bool is_diff = false, int diff_order = 1);

#endif /* TREE_OUTPUT_H */
