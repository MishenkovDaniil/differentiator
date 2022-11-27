#ifndef DEFINITIATOR_TREE_H
#define DEFINITIATOR_TREE_H

enum Type
{
    TYPE_DEFAULT = 0,
    TYPE_OP  = 1,
    TYPE_VAR = 2,
    TYPE_NUM = 3,
};

enum Op_val
{
    OP_DEFAULT = 0,
    OP_ADD = 1,
    OP_SUB = 2,
    OP_MUL = 3,
    OP_DIV = 4,
    OP_DEG = 5,
    OP_SIN = 6,
    OP_COS = 7,
};

union Value
{
    double dbl_val;
    Op_val op_val;
    char *var;
};

struct Node
{
    Type type = TYPE_DEFAULT;

    Value value = {};

    Node *left = nullptr;
    Node *right = nullptr;
    Node *parent = nullptr;
};

struct Tree
{
    Node *root = nullptr;//add debug info
};

static unsigned int DEF_ERRNO = 0;

Node *tree_create_node (Type type, const char *value, Node *left = nullptr, Node *right = nullptr);

void swap_nodes (Node **first_node, Node **second_node);

void tree_ctor (Tree *tree, unsigned int *err = &DEF_ERRNO);
void add_nodes (Tree *tree, Node *parent_node, Type left_type, Type right_type,
               char *left_value, char *right_value, unsigned int *err = &DEF_ERRNO);
void tree_dtor (Tree *tree, unsigned int *err = &DEF_ERRNO);
void tree_free (Node *node);

unsigned int tree_check (Tree *tree, unsigned int *err);
void tree_dump (Tree *tree, unsigned int *err);
void tree_graph (Tree *tree);
int make_graph_nodes (Node *node, FILE *tgraph_file);

#endif /* DEFINITIATOR_TREE_H */
