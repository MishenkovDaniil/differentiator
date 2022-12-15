#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "tree_convolution.h"
#include "io/tree_output.h"

#define Left        node->left
#define Right       node->right
#define Op          node->value.op_val

Node *tree_convolution (Node *node)
{
    int is_to_free_children = false;

    if (node->type == TYPE_OP)
    {
        if (is_op (Left))
        {
            tree_convolution (Left);
        }
        if (is_op (Right))
        {
            tree_convolution (Right);
        }

        bool is_num_left = is_num (Left);
        bool is_num_right = is_num (Right);

        if (is_num_left && is_num_right || ((Op == OP_SIN || Op == OP_COS || Op == OP_LN) && is_num_right))
        {
            double right_val = node->right->value.dbl_val;
            double left_val = 0;

            if (is_num_left)
            {
                left_val = node->left->value.dbl_val;
            }

            if (!(convolute_nums (node, left_val, right_val)))
            {
                return nullptr;
            }
        }
        else if (is_num_left)
        {
            convolute_num (node, node->left, node->right);
        }
        else if (is_num_right)
        {
            convolute_num (node, node->right, node->left);
        }
    }

    return node;
}

Node *convolute_nums (Node *node, double left_val, double right_val)
{
    node->type = TYPE_NUM;

    switch (node->value.op_val)
    {
        case OP_MUL:
        {
            node->value.dbl_val = left_val * right_val;
            break;
        }
        case OP_ADD:
        {
            node->value.dbl_val = left_val + right_val;
            break;
        }
        case OP_SUB:
        {
            node->value.dbl_val = left_val - right_val;
            break;
        }
        case OP_DIV:
        {
            if (right_val)
            {
                node->value.dbl_val = left_val / right_val;
            }
            else
            {
                debug_print ("Error: impossible operation: division by zero\n");

                node->left = nullptr;
                node->left->parent = nullptr;
            }

            break;
        }
        case OP_DEG:
        {
            node->value.dbl_val = pow (left_val, right_val);
            break;
        }
        case OP_SIN:
        {
            node->value.dbl_val = sin (right_val);
            break;
        }
        case OP_COS:
        {
            node->value.dbl_val = cos (right_val);
            break;
        }
        case OP_LN:
        {
            node->value.dbl_val = log (right_val);
            break;
        }
        default:
        {
            debug_print ("Error: wrong operation type, node %p, op_type %d", node, node->value.op_val);
            return nullptr;
        }
    }

    tree_free (Left);
    tree_free (Right);

    Left = Right = nullptr;

    return node;
}

Node *convolute_num (Node *node, Node *num_node_child, Node *not_num_node_child)
{
    bool is_to_free_children = false;

    double val = num_node_child->value.dbl_val;

    if (val == 0)
    {
        if ((Op == OP_ADD || Op == OP_SUB) && node->parent != nullptr)
        {
            if (node->parent->left == node)
            {
                node->parent->left = not_num_node_child;
            }
            else
            {
                node->parent->right = not_num_node_child;
            }

            tree_free (num_node_child);

            Left = Right = nullptr;
            tree_free (node);
        }
        else if (Op == OP_MUL)
        {
            node->type = TYPE_NUM;
            node->value.dbl_val = 0;

            is_to_free_children = true;//
        }
        else if (Op == OP_DEG)
        {
            node->type = TYPE_NUM;
            node->value.dbl_val = 1;

            is_to_free_children = true;//
        }
        else if (Op == OP_DIV)
        {
            debug_print ("Error: impossible operation: division by zero\n");

            node->left = nullptr;
            node->left->parent = nullptr;
        }
    }
    else if (val == 1)
    {
        if ((Op == OP_MUL || Op == OP_DIV || Op == OP_DEG) && node->parent != nullptr)
        {
            if (not_num_node_child)
            {
                not_num_node_child->parent = node->parent;

                if (node->parent->left == node)
                {
                    node->parent->left = not_num_node_child;
                }
                else
                {
                    node->parent->right = not_num_node_child;
                }

                tree_free (num_node_child);

                Left = Right = nullptr;
                tree_free (node);
            }
        }
    }

    if (is_to_free_children)
    {
        tree_free (Left);
        tree_free (Right);

        Left = Right = nullptr;
    }

    return nullptr;
}

int is_num (Node *node)
{
    if (node == nullptr)
    {
        return false;
    }

    return node->type == TYPE_NUM;
}

int is_op (Node *node)
{
    if (node == nullptr)
    {
        return false;
    }

    return node->type == TYPE_OP;
}

#undef Left
#undef Right
#undef Op
#undef debug_print
#undef tex_print
