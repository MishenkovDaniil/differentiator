#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "tree_convolution.h"

#define debug_print(...)                                                        \
do                                                                              \
{                                                                               \
    printf (__VA_ARGS__);                                                       \
    fprintf (stderr, ", func %s in file %s.\n", __PRETTY_FUNCTION__, __FILE__); \
}while (0)

#define create_num(num)                 tree_create_node (TYPE_NUM, #num)
#define Left                            node->left
#define Right                           node->right
#define dL                              tree_diff (Left, tex_file)
#define dR                              tree_diff (Right, tex_file)
#define cL                              cpy_node (Left)
#define cR                              cpy_node (Right)
#define ADD(left_node, right_node)      tree_create_node (TYPE_OP, "OP_ADD", left_node, right_node)
#define SUB(left_node, right_node)      tree_create_node (TYPE_OP, "OP_SUB", left_node, right_node)
#define MUL(left_node, right_node)      tree_create_node (TYPE_OP, "OP_MUL", left_node, right_node)
#define DIV(left_node, right_node)      tree_create_node (TYPE_OP, "OP_DIV", left_node, right_node)
#define DEG(left_node, right_node)      tree_create_node (TYPE_OP, "OP_DEG", left_node, right_node)
#define SIN(node)                       tree_create_node (TYPE_OP, "OP_SIN", node)
#define COS(node)                       tree_create_node (TYPE_OP, "OP_COS", node)

Node *tree_convolution (Node *node)
{
    int is_to_free_children = false;

    Node *left_node = node->left;
    Node *right_node = node->right;

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

        if (is_num_left && is_num_right)
        {
            double left_val = node->left->value.dbl_val;
            double right_val = node->right->value.dbl_val;

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
    Node *left_node = node->left;
    Node *right_node = node->right;
    //is_to_free_children = true;

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
        default:
        {
            debug_print ("Error: wrong operation type, node %p, op_type %d", node, node->value.op_val);
            return nullptr;
        }
    }

    tree_free (left_node);
    tree_free (right_node);

    node->left = nullptr;
    node->right = nullptr;

    return node;
}

Node *convolute_num (Node *node, Node *num_node_child, Node *not_num_node_child)
{
    bool is_to_free_children = false;

    Node *left_node = node->left;
    Node *right_node = node->right;

    double val = num_node_child->value.dbl_val;

    if (val == 0)
    {
        if ((node->value.op_val == OP_ADD || node->value.op_val == OP_SUB) && node->parent != nullptr)
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

            node->left = node->right = nullptr;
            tree_free (node);
        }
        else if (node->value.op_val == OP_MUL)
        {
            node->type = TYPE_NUM;
            node->value.dbl_val = 0;

            is_to_free_children = true;//
        }
        else if (node->value.op_val == OP_DEG)
        {
            node->type = TYPE_NUM;
            node->value.dbl_val = 1;

            is_to_free_children = true;//
        }
        else if (node->value.op_val == OP_DIV)
        {
            debug_print ("Error: impossible operation: division by zero\n");

            node->left = nullptr;
            node->left->parent = nullptr;
        }
        else
        {
            debug_print ("Error: wrong operation type, node %p, op_type %d", node, node->value.op_val);
            return nullptr;
        }
    }
    else if (val == 1)
    {
        if (node->value.op_val == OP_MUL || node->value.op_val == OP_DIV || node->value.op_val == OP_DEG)
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

                node->left = node->right = nullptr;
                tree_free (node);
            }
        }
        else
        {
            debug_print ("Error: wrong operation type, node %p, op_type %d", node, node->value.op_val);
            return nullptr;
        }
    }

    if (is_to_free_children)
    {
        tree_free (left_node);
        tree_free (right_node);

        node->left = nullptr;
        node->right = nullptr;
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

#undef create_num
#undef Left
#undef Right
#undef dL
#undef dR
#undef cL
#undef cR
#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef DEG
#undef SIN
#undef COS