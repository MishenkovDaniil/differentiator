#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../tree.h"
#include "tree_diff.h"
#include "func_value.h"
#include "decomposition.h"
#include "../tree_convolution.h"
#include "../io/tree_output.h"

#define create_num(num)                 tree_create_num (num)
#define create_var(var)                 tree_create_node (TYPE_VAR, #var)
#define ADD(left_node, right_node)      tree_create_node (TYPE_OP, "OP_ADD", left_node, right_node)
#define SUB(left_node, right_node)      tree_create_node (TYPE_OP, "OP_SUB", left_node, right_node)
#define MUL(left_node, right_node)      tree_create_node (TYPE_OP, "OP_MUL", left_node, right_node)
#define DIV(left_node, right_node)      tree_create_node (TYPE_OP, "OP_DIV", left_node, right_node)
#define DEG(left_node, right_node)      tree_create_node (TYPE_OP, "OP_DEG", left_node, right_node)

Node *tangent (const Tree *func, double point)
{
    double diff_point_val = node_point_value (tree_diff (func->root), point);
    double point_val = node_point_value (func->root, point);

    Node *tangent_node = ADD (create_num (point_val), MUL (create_num (diff_point_val), SUB (create_var(x), create_num (point))));

    return tangent_node;
}


int factorial (int number)
{
    int result = 1;

    for (int i = 1; i <= number; i++)
    {
        result *= i;
    }

    return result;
}

Node *decompose (Tree *taylor_tree, const Node *node, int decompose_deg, double point)
{
    int diff_order = 0;

    double *values = (double *)calloc (decompose_deg + 1, sizeof (double));

    values[0] = node_point_value (node, point);

    Node *diff_tree_root = tree_diff (node);
    Node *taylor = create_num (values[0]);

    int max_diff_order = decompose_deg;

    for (int i = 1; i <= max_diff_order; i++)
    {
        values[i] = node_point_value (diff_tree_root, point);

        if (i == 1)
        {
            taylor = ADD (taylor, MUL(create_num (values[1]), SUB (create_var (x), create_num (point))));
        }
        else
        {
            taylor = ADD (taylor, MUL (DIV (create_num (values[i]), create_num (factorial (i))), DEG (SUB (create_var (x), create_num (point)), create_num (i))));
        }

        Node *temp_node = diff_tree_root;

        if (i < decompose_deg)
        {
            tree_convolution (diff_tree_root);
            diff_tree_root = tree_diff (diff_tree_root);
        }

        tree_free (temp_node);
    }

    tree_convolution (taylor);
    taylor_tree->root = taylor;

    free (values);

    return taylor;
}

#undef create_num
#undef create_var
#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef DEG
#undef debug_print
#undef tex_print
