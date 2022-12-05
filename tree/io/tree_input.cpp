#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>

#define create_num(num)                 tree_create_node (TYPE_NUM, #num)
#define Left                            node->left
#define Right                           node->right
#define cL                              cpy_node (Left)
#define cR                              cpy_node (Right)
#define ADD(left_node, right_node)      tree_create_node (TYPE_OP, "OP_ADD", left_node, right_node)
#define SUB(left_node, right_node)      tree_create_node (TYPE_OP, "OP_SUB", left_node, right_node)
#define MUL(left_node, right_node)      tree_create_node (TYPE_OP, "OP_MUL", left_node, right_node)
#define DIV(left_node, right_node)      tree_create_node (TYPE_OP, "OP_DIV", left_node, right_node)
#define DEG(left_node, right_node)      tree_create_node (TYPE_OP, "OP_DEG", left_node, right_node)
#define SIN(node)                       tree_create_node (TYPE_OP, "OP_SIN", nullptr, node)
#define COS(node)                       tree_create_node (TYPE_OP, "OP_COS", nullptr, node)

#include "../tree.h"
#include "tree_input.h"
#include "tree_output.h"
#include "../../config.h"

bool get_config (Config *config)
{
    printf ("Enter point a to find f(a):");

    if (scanf (" %lf", &config->point) == 0)
    {
        fprintf (stderr, "Error: bad input data (func point).\n");
        return false;
    }

    printf ("Enter diff order to find: ");

    if (scanf (" %d", &config->diff_order) == 0)
    {
        fprintf (stderr, "Error: bad input data (diff order).\n");
        return false;
    }

    printf ("Enter decomposition order and point of decomposition: ");

    if (scanf (" %d %lf", &config->decompose_order, &config->decompose_point) < 2)
    {
        fprintf (stderr, "Error: bad input data (config for decompose func).\n");
        return false;
    }

    if (config->diff_order <= 0 || config->decompose_order <= 0)
    {
        printf ("Error: diff order must be above zero");
        return false;
    }

    return true;
}

int get_file_size (const char *file_name)               ///repo onegin ../io/io.cpp
{
    struct stat buf = {};

    stat(file_name, &buf);
    int errnosave = 0;

    if (errno)
    {
        errnosave = errno;
        perror ("stat() failed");

        return errnosave;
    }

    return buf.st_size;
}

Node *tree_fill (Tree *tree, FILE *input_file, const char *file_name)
{
    int expr_size = get_file_size (file_name);
    char *expr = (char *)calloc (expr_size, sizeof (char));

    fread (expr, sizeof (char), expr_size, input_file);

    GetNodeG (tree, expr);

    if (!(tree->root))
    {
        debug_print ("Error: failed to read from input file");
    }

    return tree->root;
}

void skip_spaces (const char **str)
{
    while (isspace (**str))
    {
        (*str)++;
    }
}

Node *GetNodeG (Tree *tree, const char *expr)
{
    skip_spaces (&expr);
    tree->root = GetNodeE (&expr);
    skip_spaces (&expr);

    if (!(*expr == '\0'))
    {
        debug_print ("Error: last symbol to read is not '\\0' (expr is {%s} now)", expr);
        return nullptr;
    }

    return tree->root;
}

Node *GetNodeE (const char **expr)
{
    skip_spaces (expr);

    Node *result = GetNodeT (expr);
    Node *right_node = nullptr;

    skip_spaces (expr);

    while (**expr == '+' || **expr == '-')
    {
        const char op = **expr;

        (*expr)++;

        right_node = GetNodeT (expr);
        skip_spaces (expr);

        if (op == '+')
        {
            result = ADD (result, right_node);
        }
        else
        {
            result = SUB (result, right_node);
        }
    }

    return result;
}

Node *GetNodeT (const char **expr)
{
    skip_spaces (expr);

    Node *result = GetNodeD (expr);
    Node *right_node = nullptr;

    skip_spaces (expr);

    while (**expr == '*' || **expr == '/')
    {
        const char op = **expr;

        (*expr)++;

        right_node = GetNodeD (expr);

        skip_spaces (expr);

        if (op == '*')
        {
            result = MUL (result, right_node);
        }
        else
        {
            result = DIV (result, right_node);
        }
    }

    return result;
}

Node *GetNodeD (const char **expr)
{
    skip_spaces (expr);

    Node *result = GetNodeP (expr);
    Node *right_node = nullptr;

    skip_spaces (expr);

    while (**expr == '^')
    {
        (*expr)++;

        right_node = GetNodeP (expr);

        skip_spaces (expr);

        result = DEG (result, right_node);
    }

    return result;
}

Node *GetNodeP (const char **expr)
{
    skip_spaces (expr);

    Node *result = nullptr;

    if (**expr == '(')
    {
        (*expr)++;

        result = GetNodeE (expr);

        skip_spaces (expr);

        if (!(**expr == ')'))
        {
            debug_print ("Error: no closing bracket (after opening one) (expr is {%s} now)", *expr);
            return nullptr;
        }

        (*expr)++;
    }
    else if (!(result = GetNodeV (expr)))
    {
        result = GetNodeN (expr);
    }

    return result;
}

Node *GetNodeN (const char **expr)
{
    skip_spaces (expr);

    double value = 0;

    const char *old_expr = *expr;
    char *new_expr = nullptr;
    value = strtod (*expr, &new_expr);
    *expr = new_expr;

    Node *result = create_num (value);
    result->value.dbl_val = value;

    if (!(*expr != old_expr))
    {
        debug_print ("Error: syntax error (expr is {%s} now)", *expr);
        return nullptr;
    }

    return result;
}

Node *GetNodeV (const char **expr)
{
    skip_spaces (expr);

    Node *result = nullptr;
    Node *left_node = nullptr;

    char var[4] = "";

    int var_len = 0;

    while (isalpha (**expr))
    {
        *(var + var_len) = **expr;

        var_len++;
        (*expr)++;
    }

    if (var_len)//and var != log ln cos sin tg arctg arcsin arccos arcctg ctg e pi
    {
        if (var_len > 3)
        {
            printf ("Error: max var len is 3");
            return nullptr;
        }
        else if (strcasecmp (var, "sin") == 0)
        {
            left_node = GetNodeP (expr);

            result = SIN (left_node);
        }
        else if (strcasecmp (var, "cos") == 0)
        {
            left_node = GetNodeP (expr);

            result = COS (left_node);
        }
        else
        {
            result = tree_create_node (TYPE_VAR, var);
        }
    }

    return result;
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
#undef debug_print
#undef tex_print
