#ifndef TREE_TEX_COMPRESSION_H
#define TREE_TEX_COMPRESSION_H

#include "tree.h"

Tree *tree_compression (Tree *src_tree, Tree *dst_tree, Node *compress_node = nullptr);
size_t node_len (Node *node);
unsigned long node_hash (Node *node);
size_t get_num_len (double num);
Node *find_node (Tree *src_tree, Tree *dst_tree, Node *src_node);

#endif /* TREE_TEX_COMPRESSION_H */
