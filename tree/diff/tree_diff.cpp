#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../tree.h"
#include "tree_diff.h"
#include "../io/tree_output.h"

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

Node *tree_diff (const Node *node, FILE *tex_file, unsigned int *err)
{
    assert (node);
    assert (err);

    Node *result = nullptr;

    if (node == nullptr)
    {
        printf ("Error: nullptr node, file: %s, function: %s.\n", __FILE__, __PRETTY_FUNCTION__);
        return nullptr;
    }
    if (err == nullptr)
    {
        printf ("Error: err is nullptr, file: %s, function: %s.\n", __FILE__, __PRETTY_FUNCTION__);
    }
    if (*err)
    {
        printf ("err\n");
        return nullptr;
    }

    switch (node->type)
    {
        case TYPE_DEFAULT:
        {
            return nullptr;
        }
        case TYPE_NUM:
        {
            result = create_num (0);
            tree_tex_print (node, result, tex_file);

            return result;
        }
        case TYPE_VAR:
        {
            result = create_num (1);
            tree_tex_print (node, result, tex_file);

            return result;
        }
        case TYPE_OP:
        {
            switch (node->value.op_val)
            {
                case OP_DEFAULT:
                {
                    return nullptr;
                }
                case OP_ADD:
                {
                    result =  ADD (dL, dR);
                    tree_tex_print (node, result, tex_file);

                    return result;
                }
                case OP_SUB:
                {
                    result = SUB (dL, dR);
                    tree_tex_print (node, result, tex_file);

                    return result;
                }
                case OP_MUL:
                {
                    result = ADD (MUL (dL, cR), MUL (cL, dR));
                    tree_tex_print (node, result, tex_file);

                    return result;
                }
                case OP_DIV:
                {
                    result = DIV (SUB (MUL (dL, cR), MUL (cL, dR)), MUL (cR, cR));
                    tree_tex_print (node, result, tex_file);

                    return result;
                }
                default:
                {
                    printf ("Error: unknown operation node: %p, operation: %d, file: %s, function: %s.\n",
                            node, node->value.op_val, __FILE__, __PRETTY_FUNCTION__);

                    return nullptr;
                }
            }
        }
        default:
        {
            return nullptr;
        }
    }
}

Node *cpy_node (const Node *node)
{
    Node *copy = tree_create_node (TYPE_DEFAULT, nullptr);

    memcpy (copy, node, sizeof (Node));

    if (copy == nullptr)
    {
        printf ("copy failed\n");
    }

    return copy;
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
