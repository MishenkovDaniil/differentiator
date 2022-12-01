#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#include "../tree.h"
#include "tree_output.h"
#include "../../../standart_functions/io/io.h"

#define debug_print(...)                                                        \
do                                                                              \
{                                                                               \
    printf (__VA_ARGS__);                                                       \
    fprintf (stderr, ", func %s in file %s.\n", __PRETTY_FUNCTION__, __FILE__); \
}while (0)

#define tex_print(...) fprintf (tex_file, __VA_ARGS__)

void tree_tex_print (const Node *left_part, const Node *right_part, FILE *tex_file, bool is_diff, int diff_order)
{
    assert (tex_file);

    tex_print ("\\[");

    if (is_diff && left_part)
    {
        tex_print ("(");

        tex_print_node (left_part, tex_file);

        if (diff_order == 1)
        {
            tex_print (")' = ");
        }
        else
        {
            tex_print (")^{(%d)} = ", diff_order);
        }
    }
    else if (left_part)
    {
        tex_print_node (left_part, tex_file);
        tex_print (" = ");
    }

    tex_print_node (right_part, tex_file);

    tex_print ("\\]\\newline\n");
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

                    if (node->right->type == TYPE_OP)
                    {
                        tex_print ("(");
                        tex_print_node (node->right, tex_file);
                        tex_print (")");
                    }
                    else
                    {
                        tex_print_node (node->right, tex_file);
                    }

                    break;
                }
                case OP_MUL:
                {
                    if (node->left->type == TYPE_OP)
                    {
                        if (node->left->value.op_val == OP_ADD || node->left->value.op_val == OP_SUB)
                        {
                            tex_print ("(");
                            tex_print_node (node->left, tex_file);
                            tex_print (")");
                        }
                        else
                        {
                            tex_print_node (node->left, tex_file);
                        }
                    }
                    else
                    {
                        tex_print_node (node->left, tex_file);
                    }

                    tex_print ("\\times");

                    if (node->right->type == TYPE_OP)
                    {
                        if (node->right->value.op_val == OP_ADD || node->right->value.op_val == OP_SUB)
                        {
                            tex_print ("(");
                            tex_print_node (node->right, tex_file);
                            tex_print (")");
                        }
                        else
                        {
                            tex_print_node (node->right, tex_file);
                        }
                    }
                    else
                    {
                        tex_print_node (node->right, tex_file);
                    }

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
                    if (node->left->type == TYPE_OP)
                    {
                        tex_print ("(");

                        tex_print_node (node->left, tex_file);

                        tex_print (")");
                    }
                    else
                    {
                        tex_print_node (node->left, tex_file);
                    }

                    tex_print ("^{");

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

void print_tex_header (FILE *tex_file)
{
    fprintf (tex_file,  "\\documentclass{article}\n"
                        "\\usepackage[utf8]{inputenc}\n"
                        "\\usepackage{amsmath}\n"
                        "\\usepackage[english, russian]{babel}\n"
                        "\\usepackage{euscript}\n"
                        "\\usepackage{mathrsfs}\n"
                        "\\usepackage{amssymb}\n"
                        "\\usepackage{graphicx}\n"
                        "\\usepackage[12pt]{extsizes}\n"
                        "\\renewcommand{\\familydefault}{\\rmdefault}\n"
                        "\\title{\\textbf {Идеи решения задач тысячелетия путем дифференцированного исчисления}}\n"
                        "\\author{Мишенков Даниил Николаевич,\\\\\n"
                        "\t\tдоцент кафедры высшей философии МГУ}\n"
                        "\\date{\\textit {\\normalsize {ноябрь 2022}}}\n"
                        "\\begin{document}\n"
                        "\\maketitle\n");
}

void new_section (FILE *tex_file)
{
    tex_print ("\\section {}\n");
}

void tex_print_graphic (FILE *graphic_file, const char *graphic_file_name, const char *input_file_name,
                        Tree *func, FILE *tex_file)
{
    int func_len = get_file_size (input_file_name);

    char *graphic_func = (char *)calloc (func_len + 100, sizeof (char));
    func_for_gnuplot (func->root, graphic_func);
    printf ("\n[%s]\n", graphic_func);
    fprintf  (graphic_file, "set terminal png size 800, 600\n"
                            "set xlabel \"x\"\n"
                            "set ylabel \"y\"\n"
                            "set grid\n"
                            "set output \"func.png\"\nplot %s", graphic_func);

    fflush (graphic_file);
    system ("gnuplot graphic.txt");

    tex_print ("\\begin{figure}[h]\n"
               "\\centering\n"
               "\\includegraphics[width=0.8\\linewidth]{func.png}\n"
               "\\caption{график функций полученный после комплексного анализа}\n"
               "\\label{fig:mpr}\n"
               "\\end{figure}\n");

    free (graphic_func);
}

bool func_for_gnuplot (Node *func_node, char *graphic_func)
{
    static int shift = 0;
    int temp = 0;

    if (func_node == nullptr)
    {
        return false;
    }

    if (func_node->left)
    {
        sprintf (graphic_func + shift++, "(");

        if (func_for_gnuplot (func_node->left, graphic_func) == false)
        {
            return false;
        }

        sprintf (graphic_func + shift++, ")");
    }

    switch (func_node->type)
    {
        case TYPE_DEFAULT:
        {
            break;
        }
        case TYPE_NUM:
        {
            int value = func_node->value.dbl_val;

            sprintf (graphic_func + shift, "%.2lf", func_node->value.dbl_val);

            shift += 3;

            while (value)
            {
                shift++;
                value /= 10;
            }

            break;
        }
        case TYPE_VAR:
        {
            sprintf (graphic_func + shift, "%s", func_node->value.var);
            shift += strlen (func_node->value.var);

            break;
        }
        case TYPE_OP:
        {
            switch (func_node->value.op_val)
            {
                case OP_DEFAULT:
                {
                    break;
                }
                case OP_ADD:
                {
                    sprintf (graphic_func + shift++, "+");

                    break;
                }
                case OP_SUB:
                {
                    sprintf (graphic_func + shift++, "-");

                    break;
                }
                case OP_MUL:
                {
                    sprintf (graphic_func + shift++, "*");

                    break;
                }
                case OP_DIV:
                {
                    sprintf (graphic_func + shift++, "/");

                    break;
                }
                case OP_DEG:
                {
                    sprintf (graphic_func + shift, "**");
                    shift += 2;

                    break;
                }
                case OP_SIN:
                {
                    sprintf (graphic_func + shift, "sin ");
                    shift += 4;

                    break;
                }
                case OP_COS:
                {
                    sprintf (graphic_func + shift, "cos ");
                    shift += 4;

                    break;
                }
                default:
                {
                    debug_print ("Error: unknown operation type %d", func_node->value.op_val);
                    return false;
                    break;
                }

            }

            break;
        }
        default:
        {
            debug_print ("Error: unknown node type %d", func_node->type);
            return false;
            break;
        }
    }

    if (func_node->right)
    {
        sprintf (graphic_func + shift++, "(");

        func_for_gnuplot (func_node->right, graphic_func);

        sprintf (graphic_func + shift++, ")");
    }

    return true;
}

void print_original_function (const char *func_name, Tree *func, FILE *tex_file)
{
    tex_print ("Рассмотрим следующую функцию:\\newline\n");

    print_function (func_name, func, tex_file);

    tex_print ("Очевидно, что понять логику ее работы просто так невозможно, поэтому "
               "необходимо провести комплексный анализ данной функции.\\newline\\newline\n");
}

void print_function (const char *func_name, Tree *func, FILE *tex_file)
{
    unsigned int err = 0;

    tree_check (func, &err);

    tex_print ("\\[%s(x) = ", func_name);
    tex_print_node (func->root, tex_file);
    tex_print ("\\]\\newline\n");
}

void print_diff_func (const char *func_name, Tree *diff_func, FILE *tex_file, int diff_order)
{
    unsigned int err = 0;
    tree_check (diff_func, &err);

    if (diff_order == 1)
    {
        tex_print ("\\[%s'(x) = ", func_name);
    }
    else
    {
        tex_print ("\\[%s^{(%d)}(x) = ", func_name, diff_order);
    }

    tex_print_node (diff_func->root, tex_file);

    tex_print ("\\]\\newline\n");
}

void print_point_value (const char *func_name, double point, double point_value, FILE *tex_file)
{
    tex_print ("Найдем значение функции в точке %.2lf знаменитым в 17 веке методом буль-буль"
                       " (aka вы увидите только ответ).\\newline", point);

    if (point_value == NAN)
    {
        tex_print ("функция не определена в точке a = %.2lf.\\newline", point);
    }
    else
    {
        tex_print ("\\[%s(a) = %.2lf, где a = %.2lf\\]\\newline", func_name, point_value, point);
    }
}

void print_decompose_tree (const char *func_name, Tree *decompose_tree, double decompose_point, int decompose_order,
                           FILE *tex_file)
{
    unsigned int err = 0;
    tree_check (decompose_tree, &err);

    tex_print ("Разложим функцию (по Тейлору) до $o(x^{%d})$ в точке а = %.2lf\\newline\n", decompose_order, decompose_point);

    tex_print ("f(a) = ");

    tree_tex_print (nullptr, decompose_tree->root, tex_file);
}

void print_tex_ending (FILE *tex_file)
{
    fprintf (tex_file, "\\end {document}");
}
#undef tex_print
