#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "io/tree_output.h"
#include "tree_compression.h"
#include "diff/tree_diff.h"
#include "../../Stack/stack/stack.h"

bool find_node_path (Stack *stk, Node *cur_node, Node *search_node);

Tree *tree_compression (Tree *src_tree, Tree *dst_tree, Node *compress_node)
{
    assert (src_tree);
    assert (dst_tree);
    assert (src_tree->root);

    static int index = 1;

    size_t max_len = 40;
    bool is_compressed = false;

    if (!(compress_node))
    {
        compress_node = src_tree->root;
    }

    if (compress_node->type != TYPE_OP)
    {
        return nullptr;
    }

    size_t len_1 = 0;
    size_t len_2 = 0;

    len_1 = node_len (compress_node->left);
    len_2 = node_len (compress_node->right);

    if (compress_node->value.op_val == OP_DIV && (len_1 > 2*max_len || len_2 > 2*max_len))
    {
        max_len *= 2;
    }

    printf ("\nlen1 = [%d], len2 = %d\n", len_1, len_2);

    Node *dst_compress_node = nullptr;

    if ((len_1 > max_len || len_2 > max_len) && is_compressed == false)
    {
        dst_tree->root = cpy_node (src_tree->root);
        dst_compress_node = find_node (src_tree, dst_tree, compress_node);
        assert (dst_compress_node);
    }
    else if (len_1 > max_len || len_2 > max_len)
    {
        dst_compress_node = find_node (src_tree, dst_tree, compress_node);
        assert (dst_compress_node);
    }

    if ((len_1 > max_len) && (len_1 < 2*max_len))// && (tree_compression (src_tree, dst_tree, compress_node->left) == nullptr))
    {
        assert (dst_compress_node);

        char var[10] = "";

        sprintf (var, "A_%d", index++);

        dst_compress_node->left->type = TYPE_VAR;
        dst_compress_node->left->value.var = (char *)calloc (6, sizeof (char));
        strcpy (dst_compress_node->left->value.var, var);

        is_compressed = true;
    }
    else if (len_1 > 2*max_len)
    {
        tree_compression (src_tree, dst_tree, compress_node->left);
        is_compressed = true;
    }

    if (len_2 > max_len && len_2 < 2*max_len)// && (tree_compression (src_tree, dst_tree, compress_node->right) == nullptr))
    {
        assert (dst_compress_node);

        char var[6] = "";
        sprintf (var, "A_%d", index++);

        dst_compress_node->right->type = TYPE_VAR;
        dst_compress_node->right->value.var = (char *)calloc (6, sizeof (char));
        strcpy (dst_compress_node->right->value.var, var);

        is_compressed = true;
    }
    else if (len_2 > 2*max_len)
    {
            tree_compression (src_tree, dst_tree, compress_node->right);

            is_compressed = true;
    }

    if (!(is_compressed))
    {
        return nullptr;
    }

    fprintf (stderr, "line %d\n", __LINE__);

    return dst_tree;
}


Node *find_node (Tree *src_tree, Tree *dst_tree, Node *src_node)
{
    assert (src_tree);
    assert (dst_tree);
    assert (src_node);

    Stack stk = {};
    stack_init (&stk, 10);
    Node *dst_node = dst_tree->root;

    find_node_path (&stk, src_tree->root, src_node);

    while (stk.size-- > 0)
    {
        if ((stack_pop (&stk)) == 0)
        {
            dst_node = dst_node->left;
        }
        else
        {
            dst_node = dst_node->right;
        }
        assert (dst_node);
    }

    stack_dtor (&stk);

    return dst_node;
}

bool find_node_path (Stack *stk, Node *cur_node, Node *search_node)
{
    if (cur_node == nullptr)
    {
        return false;
    }

    if (cur_node == search_node)
    {
        return true;
    }

    if ((find_node_path (stk, cur_node->left, search_node)) == true)
    {
        stack_push (stk, 0);
    }
    else if ((find_node_path (stk, cur_node->right, search_node)) == true)
    {
        stack_push (stk, 1);
    }
    else
    {
        return false;
    }

    return true;
}


size_t node_len (Node *node)
{
    if (node == nullptr)
    {
        return 0;
    }

    switch (node->type)
    {
        case TYPE_DEFAULT:
        {
            return 0;
        }
        case TYPE_NUM:
        {
            return get_num_len (node->value.dbl_val);
        }
        case TYPE_VAR:
        {
            return strlen (node->value.var) + 1;
        }
        case TYPE_OP:
        {
            switch (node->value.op_val)
            {
                case OP_DEFAULT:
                {
                    return 0;
                }

                #define DEF_OP(name, num, sign, len,...)                \
                case OP_##name:                                         \
                {                                                       \
                    size_t l_len = node_len (node->left);               \
                    size_t r_len = node_len (node->right);              \
                                                                        \
                    if (num != OP_DIV)                                  \
                    {                                                   \
                        return l_len + len + r_len;                     \
                    }                                                   \
                                                                        \
                    return (l_len > r_len) ? l_len + len : r_len + len; \
                }

                #include "../operations.h"

                default:
                {
                    debug_print ("Error: impossible node op type %d, node %p, see .html for more info.\n",
                                 node->value.op_val, node);
                    return 0;
                }

                #undef DEF_OP
            }
        }
        default:
        {
            debug_print ("Error: impossible node type %d, node %p, see .html for more info.\n", node->type, node);
            return 0;
        }
    }
}

size_t get_num_len (double num)
{
    size_t len = 1;

    if (num < 0)
    {
        len++;
    }

    while (num > 9)
    {
        num /= 10;
        len++;
    }

    return len + 3;
}


unsigned long node_hash (Node *node)
{
    return 0;
}
