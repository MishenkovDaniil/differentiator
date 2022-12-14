#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

Node *tree_create_node (Type type, const char *value, Node *left, Node *right)
{
    Node *node = (Node *)calloc (1, sizeof (Node));
    assert (node);

    if (node == nullptr)
    {
        return printf ("calloc failed"), nullptr;
    }

    switch (type)
    {
        case TYPE_DEFAULT: break;
        case TYPE_OP:
        {
            node->type = TYPE_OP;

            if (strcasecmp ("OP_DEFAULT", value) == 0)
            {
                node->value.op_val = OP_DEFAULT;
            }

            #define DEF_OP(name, num,...)                   \
            else if (strcasecmp ("OP_"#name, value) == 0)   \
            {                                               \
                node->value.op_val = OP_##name;             \
            }

            #include "../operations.h"

            #undef DEF_OP

            else
            {
                printf ("Error: wrong value type %d", type);
                return nullptr;
            }

            break;
        }
        case TYPE_VAR:
        {
            node->type = TYPE_VAR;

            node->value.var = (char *)calloc (strlen (value) + 1, sizeof (char));
            strcpy (node->value.var, value);

            break;
        }
        case TYPE_NUM:
        {
            node->type = TYPE_NUM;

            node->value.dbl_val = atof (value);

            break;
        }
        default:
        {
            printf ("Error: wrong value type %d", type);
            return nullptr;
        }
    }

    node->left = left;
    node->right = right;

    if (node->left)
    {
        node->left->parent = node;
    }
    if (node->right)
    {
        node->right->parent = node;
    }

    return node;
}

Node *tree_create_num (double val, Node *left, Node *right)
{
    Node *node = (Node *)calloc (1, sizeof (Node));
    assert (node);

    if (node == nullptr)
    {
        return printf ("calloc failed"), nullptr;
    }

    node->type = TYPE_NUM;

    node->value.dbl_val = val;

    node->left = left;
    node->right = right;

    if (node->left)
    {
        node->left->parent = node;
    }
    if (node->right)
    {
        node->right->parent = node;
    }

    return node;
}

void tree_ctor (Tree *tree, unsigned int *err)
{
    assert (tree);

    if (tree == nullptr)
    {
        printf ("wrong tree pointer");
        return;
    }
    tree->root = tree_create_node (TYPE_OP, "OP_MUL");
    assert (tree->root);
}

void add_nodes (Tree *tree, Node *parent_node, Type left_type, Type right_type, char *left_value, char *right_value, unsigned int *err)
{
    assert (tree);
    assert (parent_node);

    if (parent_node == nullptr)
    {
        printf ("TREE_ERROR: incorrect pointer to parent node.\n");

        return;
    }
    if (tree_check (tree, err))
    {
        return;
    }

    Node *new_node_left = tree_create_node (left_type, left_value);
    Node *new_node_right = tree_create_node (right_type, right_value);

    if (new_node_left == nullptr || new_node_right == nullptr)
    {
        return;
    }

    parent_node->left = new_node_left;
    parent_node->right = new_node_right;

    new_node_left->parent = parent_node;
    new_node_right->parent = parent_node;

    tree_check (tree, err);
}

void tree_dtor (Tree *tree, unsigned int *err)
{
    assert (tree && tree->root);

    if (tree && tree->root)
    {
        tree_free (tree->root);
    }
    else
    {
        printf ("nothing to dtor");
    }
}

void tree_free (Node *node)
{
    if (node->left)
    {
        //node->left->value = nullptr;
        tree_free(node->left);
    }
    if (node->right)
    {
        //node->right->data = nullptr;
        tree_free(node->right);
    }

    if (node->type == TYPE_VAR)
    {
        if (node->value.var)
        {
            free (node->value.var);
        }
    }

    node->left = nullptr;
    node->right = nullptr;
    node->parent = nullptr;

    free (node);

    node = nullptr;
}


unsigned int tree_check (Tree *tree, unsigned int *err)
{
    //if (*err)
    {
        tree_dump (tree, err);

        //return *err;
    }

    return 0;
}

void tree_dump (Tree *tree, unsigned int *err)
{
    tree_graph (tree);
}

void tree_graph (Tree *tree)
{
    static int PNG_FILE_NUMBER = 0;

    FILE *tgraph_file = fopen ("tree_graph", "w");
    FILE *tree_log  = nullptr;

    if (!(PNG_FILE_NUMBER))
    {
        tree_log  = fopen ("tree_dump.html", "w");
    }
    else
    {
        tree_log  = fopen ("tree_dump.html", "a");
    }

    fprintf (tgraph_file, "digraph\n{\n\t");

    make_graph_nodes (tree->root, tgraph_file);

    fprintf (tgraph_file, "}");
    fclose (tgraph_file);

    const int CMD_LEN = 100;
    char cmd[CMD_LEN] = "";

    #ifdef __unix__
    sprintf (cmd, "dot tree_graph -T png -o tree_dots/tree_dot%d.png", PNG_FILE_NUMBER);
    #else
    sprintf (cmd, "Dot tree_graph -T png -o tree_dots/tree_dot%d.png", PNG_FILE_NUMBER);
    #endif

    system ((const char *)cmd);

    fprintf (tree_log, "<pre>\n<img src = tree_dots/tree_dot%d.png>\n", PNG_FILE_NUMBER++);

    fclose (tree_log);
}

int make_graph_nodes (Node *node, FILE *tgraph_file)
{
    static int graph_num = 0;

    int node_num = graph_num;

    if (node == nullptr)
    {
        fprintf (tgraph_file, "node_%d [shape = record, style = \"filled\", fillcolor = \"orange\", label = \"{NULL\"];\n\t", graph_num++);
    }
    else
    {
        switch (node->type)
        {
            case TYPE_DEFAULT:
            {
                fprintf (tgraph_file, "node_%d [shape = record, style = \"filled\", fillcolor = \"orange\", label = \"{parent = %p} | {DEFAULT}\"];\n\t", graph_num++, node->parent);
                break;
            }
            case TYPE_OP:
            {
                switch (node->value.op_val)
                {
                    case OP_DEFAULT:
                    {
                        fprintf (tgraph_file, "node_%d [shape = record, style = \"filled\", fillcolor = \"orange\", label = \"{node %p | {parent = %p} | {OP | DEFAULT} | {L %p | R %p}} \"];\n\t", graph_num++, node, node->parent, node->left, node->right);
                        break;
                    }

                    #define DEF_OP(name, num, sign,...)                                                                         \
                    case OP_##name:                                                                                             \
                    {                                                                                                           \
                        fprintf (tgraph_file, "node_%d [shape = record, style = \"filled\", fillcolor = \"lightblue\","         \
                                            "label = \"{node %p | {parent = %p} | {OP | %s} | {L %p | R %p}} \"];\n\t",         \
                                            graph_num++, node, node->parent, #sign, node->left, node->right);                   \
                        break;                                                                                                  \
                    }

                    #include "../operations.h"

                    default:
                    {
                        printf ("Error: wrong node op type in %s", __PRETTY_FUNCTION__);
                        break;
                    }

                    #undef DEF_OP
                }
                break;
            }
            case TYPE_NUM:
            {
                fprintf (tgraph_file, "node_%d [shape = record, style = \"filled\", fillcolor = \"lightblue\", label = \"{node %p | {parent = %p} | {NUM | %.2lf} | {L %p | R %p}} \"];\n\t", graph_num++, node, node->parent, node->value.dbl_val, node->left, node->right);
                break;
            }
            case TYPE_VAR:
            {
                fprintf (tgraph_file, "node_%d [shape = record, style = \"filled\", fillcolor = \"lightblue\", label = \"{node %p | {parent = %p} | {VAR | %c} | {L %p | R %p}} \"];\n\t", graph_num++, node, node->parent, *(node->value.var), node->left, node->right);
                break;
            }
            default:
            {
                printf ("Error: wrong node type in %s", __PRETTY_FUNCTION__);
                break;
            }
        }
    }

    if (node->left != nullptr)
    {
        int left = make_graph_nodes (node->left, tgraph_file);

        fprintf (tgraph_file, "node_%d->node_%d;\n\t", node_num, left);
    }
    if (node->right != nullptr)
    {
        int right = make_graph_nodes (node->right, tgraph_file);

        fprintf (tgraph_file, "node_%d->node_%d;\n\t", node_num, right);
    }

    return node_num;
}
