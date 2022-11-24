#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../tree.h"
#include "tree_output.h"

void tree_tex_print (const Node *left_part, const Node *right_part, FILE *tex_file)
{
    assert (tex_file);

    static int is_adding_text = 0;

    if (is_adding_text == 0)
    {
        print_tex_header (tex_file);
        is_adding_text = 1;
    }

    fprintf (tex_file, "$(");

    tex_print_node (left_part, tex_file);
    fprintf (tex_file, ")'");
    fprintf (tex_file, " = ");

    tex_print_node (right_part, tex_file);

    fprintf (tex_file, "$\\newline\n");
}

void tex_print_node (const Node *node, FILE *tex_file)
{
    assert (tex_file);
    /*if (node->left)
    {
        tex_print_node (node->left, tex_file);
    }*/
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
            fprintf (tex_file, " %.2lf ", node->value.dbl_val);
            break;
        }
        case TYPE_VAR:
        {
            fprintf (tex_file, " %s ", node->value.var);
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
                    fprintf (tex_file, "(");
                    if (node->left)
                    {
                        tex_print_node (node->left, tex_file);
                    }
                    fprintf (tex_file, ")");

                    fprintf (tex_file, "+");

                    fprintf (tex_file, "(");
                    if (node->right)
                    {
                        tex_print_node (node->right, tex_file);
                    }
                    fprintf (tex_file, ")");
                    break;
                }
                case OP_SUB:
                {
                    fprintf (tex_file, "(");
                    if (node->left)
                    {
                        tex_print_node (node->left, tex_file);
                    }
                    fprintf (tex_file, ")");

                    fprintf (tex_file, "-");

                    fprintf (tex_file, "(");
                    if (node->right)
                    {
                        tex_print_node (node->right, tex_file);
                    }
                    fprintf (tex_file, ")");
                    break;
                }
                case OP_MUL:
                {
                    fprintf (tex_file, "(");
                    if (node->left)
                    {
                        tex_print_node (node->left, tex_file);
                    }
                    fprintf (tex_file, ")");

                    fprintf (tex_file, "\\times ");

                    fprintf (tex_file, "(");

                    if (node->right)
                    {
                        tex_print_node (node->right, tex_file);
                    }
                    fprintf (tex_file, ")");

                    break;
                }
                case OP_DIV:
                {
                    fprintf (tex_file, "\\frac");

                    fprintf (tex_file, "{");
                    if (node->left)
                    {
                        tex_print_node (node->left, tex_file);
                    }
                    fprintf (tex_file, "}");

                    fprintf (tex_file, "{");
                    if (node->right)
                    {
                        tex_print_node (node->right, tex_file);
                    }
                    fprintf (tex_file, "}");
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
    /*if (node->right)
    {
        tex_print_node (node->left, tex_file);
    }*/
}

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
