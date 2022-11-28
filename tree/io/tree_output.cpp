#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../tree.h"
#include "tree_output.h"

#define tex_print(...) fprintf (tex_file, __VA_ARGS__)

void tree_tex_print (const Node *left_part, const Node *right_part, FILE *tex_file, bool is_diff, int diff_order)
{
    assert (tex_file);

    tex_print ("$");

    if (is_diff)
    {
        tex_print ("(");
    }

    if (left_part)
    {
        tex_print_node (left_part, tex_file);
    }

    if (is_diff)
    {
        if (diff_order == 1)
        {
            tex_print (")'");
        }
        else
        {
            tex_print (")^{(%d)}", diff_order);
        }
    }

    if (left_part)
    {
        tex_print (" = ");
    }

    tex_print_node (right_part, tex_file);

    tex_print ("$\\newline\n");
}

void tex_print_node (const Node *node, FILE *tex_file)
{
    assert (tex_file);

    if (node == nullptr)
    {
        return;
    }
    switch (node->type)
    {
        case TYPE_DEFAULT:
        {
            break;
        }
        case TYPE_NUM:
        {
            double val = node->value.dbl_val;

            if (val - (int)val == 0)
            {
                tex_print (" %.0lf ", val);
            }
            else
            {
                tex_print (" %.2lf ", val);
            }

            break;
        }
        case TYPE_VAR:
        {
            tex_print (" %s ", node->value.var);
            break;
        }
        case TYPE_OP:
        {
            switch (node->value.op_val)
            {
                case OP_DEFAULT:
                    break;
                case OP_ADD:
                {
                    tex_print_node (node->left, tex_file);

                    tex_print ("+");

                    tex_print_node (node->right, tex_file);

                    break;
                }
                case OP_SUB:
                {
                    tex_print_node (node->left, tex_file);

                    tex_print ("-");

                    tex_print_node (node->right, tex_file);

                    break;
                }
                case OP_MUL:
                {
                    tex_print ("(");

                    tex_print_node (node->left, tex_file);

                    tex_print (")\\times (");

                    tex_print_node (node->right, tex_file);

                    tex_print (")");

                    break;
                }
                case OP_DIV:
                {
                    tex_print ("\\frac{");

                    tex_print_node (node->left, tex_file);

                    tex_print ("}{");

                    tex_print_node (node->right, tex_file);

                    tex_print ("}");

                    break;
                }
                case OP_DEG:
                {
                    tex_print ("(");

                    tex_print_node (node->left, tex_file);

                    tex_print (")^{");

                    tex_print_node (node->right, tex_file);

                    tex_print ("}");

                    break;
                }
                case OP_SIN:
                {
                    tex_print ("\\sin(");

                    tex_print_node (node->left, tex_file);

                    tex_print (")");

                    break;
                }
                case OP_COS:
                {
                    tex_print ("\\cos(");

                    tex_print_node (node->left, tex_file);

                    tex_print (")");

                    break;
                }
                default:
                {
                    printf ("Error: wrong node op type in %s", __PRETTY_FUNCTION__);
                    break;
                }
            }
            break;
        }
        default:
        {
            printf ("Error: wrong node type in %s", __PRETTY_FUNCTION__);
            break;
        }
    }
}
#undef tex_print

void print_tex_header (FILE *tex_file)
{
    fprintf (tex_file,  "\\documentclass{article}\n"
                        "\\usepackage[utf8]{inputenc}\n"
                        "\\usepackage{amsmath}\n"
                        "\\usepackage[english, russian]{babel}\n"
                        "\\usepackage{euscript}\n"
                        "\\usepackage{mathrsfs}\n"
                        "\\usepackage{amssymb}\n"
                        "\\renewcommand{\\familydefault}{\\rmdefault}\n"
                        "\\title{Идеи решения задач тысячелетия путем дифференцированного исчисления}\n"
                        "\\author{Мишенков Даниил Николаевич,\\\\\n"
                        "\t\tдоцент кафедры высшей философии МГУ}\n"
                        "\\date{ноябрь 2022}\n"
                        "\\begin{document}\n"
                        "\\maketitle\n");
}

void print_tex_ending (FILE *tex_file)
{
    fprintf (tex_file, "\\end {document}");
}
