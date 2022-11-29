#ifndef FUNC_VALUE_H
#define FUNC_VALUE_H

#include "tree.h"

double func_point_value (Tree *tree, double point);
double node_point_value (const Node *node, double point);

#endif /* FUNC_VALUE_H */
