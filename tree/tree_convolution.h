#ifndef TREE_CONVOLUTION_H
#define TREE_CONVOLUTION_H

#include "tree.h"

int is_num (Node *node);
int is_op (Node *node);
Node *tree_convolution (Node *node);
Node *convolute_nums (Node *node, double left_val, double right_val);
Node *convolute_num (Node *node, Node *num_node_child, Node *not_num_node_child);

#endif /* TREE_CONVOLUTION_H */
