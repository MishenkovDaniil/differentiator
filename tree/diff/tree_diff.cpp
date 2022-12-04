#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "../tree.h"
#include "tree_diff.h"
#include "../io/tree_output.h"
#include "../tree_convolution.h"

#define create_num(num)                 tree_create_node (TYPE_NUM, #num)
#define Left                            node->left
#define Right                           node->right
#define dL                              tree_diff (Left, tex_file, is_to_print)
#define dR                              tree_diff (Right, tex_file, is_to_print)
#define cL                              cpy_node (Left)
#define cR                              cpy_node (Right)
#define ADD(left_node, right_node)      tree_create_node (TYPE_OP, "OP_ADD", left_node, right_node)
#define SUB(left_node, right_node)      tree_create_node (TYPE_OP, "OP_SUB", left_node, right_node)
#define MUL(left_node, right_node)      tree_create_node (TYPE_OP, "OP_MUL", left_node, right_node)
#define DIV(left_node, right_node)      tree_create_node (TYPE_OP, "OP_DIV", left_node, right_node)
#define DEG(left_node, right_node)      tree_create_node (TYPE_OP, "OP_DEG", left_node, right_node)
#define SIN(node)                       tree_create_node (TYPE_OP, "OP_SIN", nullptr, node)
#define COS(node)                       tree_create_node (TYPE_OP, "OP_COS", nullptr, node)

#define debug_print(...)                                                                            \
do                                                                                                  \
{                                                                                                   \
    printf (__VA_ARGS__);                                                                           \
    fprintf (stderr, ", func %s in file %s, line %d.\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);  \
}while (0)

#define tex_print(...) fprintf (tex_file, __VA_ARGS__)

static const int PHRASES_NUMBER = 8;
static const char *phrases[PHRASES_NUMBER] = {"Очевидно, что", "В СССР следующий результат было стыдно записывать",
                                              "Будем считать верным, что", "Тогда так", "А тут так",
                                              "Более содержительным является следующий результат",
                                              "Получен важнейший результат",
                                              "Читатель, в качестве упражнения, может проверить, что"};

Node *find_diff (Node *node, FILE *tex_file, int diff_order, unsigned int *err)
{
    Node *diff_tree_root = node;

    while (diff_order > 0)
    {
        Node *temp_node = diff_tree_root;

        diff_tree_root = tree_diff (temp_node, tex_file, true, err);

        if (--diff_order)
        {
            tree_convolution (diff_tree_root);
        }

        if (temp_node != node)
        {
            tree_free (temp_node);
        }
    }

    return diff_tree_root;
}

Node *tree_diff (const Node *node, FILE *tex_file, bool is_to_print, unsigned int *err)
{
    assert (node);
    assert (err);

    srand (time(NULL));

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
            break;
        }
        case TYPE_VAR:
        {
            result = create_num (1);
            break;
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
                    break;
                }
                case OP_SUB:
                {
                    result = SUB (dL, dR);
                    break;
                }
                case OP_MUL:
                {
                    result = ADD (MUL (dL, cR), MUL (cL, dR));
                    break;
                }
                case OP_DIV:
                {
                    result = DIV (SUB (MUL (dL, cR), MUL (cL, dR)), MUL (cR, cR));
                    break;
                }
                case OP_DEG:
                {
                    result = MUL (MUL (DEG(cL, SUB (cR, create_num (1))), dL ), cR);
                    break;
                }
                case OP_SIN:
                {
                    result = MUL (COS (cR), dR);
                    break;
                }
                case OP_COS:
                {
                    result = MUL (MUL(SIN (cR), create_num(-1)), dR);
                    break;
                }
                default:
                {
                    printf ("Error: unknown operation node: %p, operation: %d, file: %s, function: %s.\n",
                            node, node->value.op_val, __FILE__, __PRETTY_FUNCTION__);
                    return nullptr;
                }
            }
            break;
        }
        default:
        {
            return nullptr;
        }
    }

    if (is_to_print)
    {
        fprintf (tex_file, "%s\\newline\n", phrases[rand() % PHRASES_NUMBER]);
        tree_tex_print (node, result, tex_file, true);
    }

    return result;
}

Node *cpy_node (const Node *node)
{
    Node *copy = tree_create_node (TYPE_DEFAULT, nullptr);
    //Node *copy = (Node *)calloc (1, sizeof (Node));
    //memcpy (copy, node, sizeof (Node));
    copy->type = node->type;
    switch (copy->type)
    {
        case TYPE_DEFAULT:
            break;
        case TYPE_OP:
        {
            copy->value.op_val = node->value.op_val;
            break;
        }
        case TYPE_NUM:
        {
            copy->value.dbl_val = node->value.dbl_val;
            break;
        }
        case TYPE_VAR:
        {
            copy->value.var = (char *)calloc (strlen (node->value.var) + 1, sizeof (char));
            strcpy (copy->value.var, node->value.var);
            break;
        }
        default:
        {
            return nullptr;
        }
    }

    if (copy == nullptr)
    {
        printf ("copy failed\n");
    }

    if (node->left) copy->left = cpy_node (node->left);
    if (node->right) copy->right = cpy_node (node->right);

    return copy;
}

#undef create_num
#undef debug_print
#undef tex_print
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
