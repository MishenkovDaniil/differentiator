#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#include "../tree.h"
#include "tree_output.h"
#include "tree_input.h"
#include "../../config.h"
#include "../diff/tree_diff.h"
#include "../tree_convolution.h"
#include "../diff/func_value.h"
#include "../diff/decomposition.h"
#include "../tree_compression.h"

static unsigned int err = 0;

void tex_print_func (const char *func_name, Tree *expr, FILE *tex_file)
{
    tree_check (expr, &err);

    print_original_function (func_name, expr, tex_file);

    tex_print ("Упростим функцию, чтобы с ней было более удобно работать.\\newline\n"
               "Получим следующий результат:\\newline\\newline\n");
    tree_convolution (expr->root);

    print_function (func_name, expr, tex_file);
}

void tex_find_point_value (Tree *expr, const char *func_name, Config *config, FILE *tex_file)
{
    double func_value = func_point_value (expr, config->point);

    print_point_value (func_name, config->point, func_value, tex_file);
}

void tex_find_diff (Tree *expr, const char *func_name, Config *config, FILE *tex_file)
{
    tex_print ("Найдем производную %d порядка этой функции методом Шварцвальда III\\newline\n", config->diff_order);

    Tree def_tree = {};

    def_tree.root = find_diff (expr->root, tex_file, config->diff_order);

    tree_check (&def_tree, &err);

    tex_print ("Для простоты запишем этот результат так:\\newline\n");

    tree_convolution (def_tree.root);

    print_diff_func (func_name, &def_tree, tex_file, config->diff_order);

    tree_dtor (&def_tree);
}

void tex_decompose_func (Tree *expr, const char *func_name, const char *graphic_file_name, const char *input_file_name,
                         Config *config, FILE *tex_file, FILE *graphic_file)
{
    Tree decompose_tree = {};
    decompose (&decompose_tree, expr->root, config->decompose_order, config->decompose_point);

    print_decompose_tree (func_name, &decompose_tree, config->decompose_point, config->decompose_order, tex_file);

    tex_print_graphic (graphic_file, graphic_file_name, input_file_name, expr, tex_file);

    tree_dtor (&decompose_tree);
}


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

void tree_tex_print_compressed (const Node *left_part, const Node *right_part, const Node *original_right_part,
                                FILE *tex_file, bool is_diff, int diff_order)
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

    tex_print_compressed_node (right_part, tex_file, original_right_part);

}

// cpu idea
void tex_print_compressed_node (const Node *node, FILE *tex_file, const Node *original_node)
{
    static const char *char_labels[20] = {};
    static const Node *node_labels[20] = {};
    static int index = 0;

    static int is_not_start = 0;
    is_not_start++;
    assert (tex_file);

    if (node == nullptr || original_node == nullptr)
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

            if (*(node->value.var) == 'A')
            {
                char_labels[index] = node->value.var;
                node_labels[index++] = original_node;
            }

            break;
        }
        case TYPE_OP:
        {
            switch (node->value.op_val)
            {
                case OP_DEFAULT:
                    break;

                #define DEF_OP(name, num, sign, sign_len, plot_sign, tex_pre_sign, tex_left_cond, tex_mid_sign, tex_right_cond, tex_end_sign)   \
                case OP_##name:                                                                                                                 \
                {                                                                                                                               \
                    tex_print (tex_pre_sign);                                                                                                   \
                                                                                                                                                \
                    if ((tex_left_cond) && node->left)                                                                                                          \
                    {                                                                                                                           \
                        tex_print ("(");                                                                                                        \
                        tex_print_compressed_node (node->left, tex_file, original_node->left);                                                  \
                        tex_print (")");                                                                                                        \
                    }                                                                                                                           \
                    else if (node->left)                                                                                                                       \
                    {                                                                                                                           \
                        tex_print_compressed_node (node->left, tex_file, original_node->left);                                                  \
                    }                                                                                                                           \
                                                                                                                                                \
                    tex_print (tex_mid_sign);                                                                                                   \
                                                                                                                                                \
                    if (tex_right_cond)                                                                                                         \
                    {                                                                                                                           \
                        tex_print ("(");                                                                                                        \
                        tex_print_compressed_node (node->right, tex_file, original_node->right);                                                \
                        tex_print (")");                                                                                                        \
                    }                                                                                                                           \
                    else                                                                                                                        \
                    {                                                                                                                           \
                        tex_print_compressed_node (node->right, tex_file, original_node->right);                                                \
                    }                                                                                                                           \
                                                                                                                                                \
                    tex_print (tex_end_sign);                                                                                                   \
                                                                                                                                                \
                    break;                                                                                                                      \
                }

                #include "../../operations.h"

                default:
                {
                    printf ("Error: wrong node op type in %s", __PRETTY_FUNCTION__);
                    break;
                }

                #undef DEF_OP
            }
            break;
        }
        default:
        {
            printf ("Error: wrong node type in %s", __PRETTY_FUNCTION__);
            break;
        }
    }

    is_not_start--;
    if (is_not_start == 0)
    {
        tex_print ("\\]\\newline\n");
        tex_print ("где:");

        for (int i = 0; i < index; i++)
        {
            if (char_labels[i] && node_labels[i])
            {
                tex_print ("\\[%s = ", char_labels[i]);
                tex_print_node (node_labels[i], tex_file);
                tex_print ("\\]\n");

                char_labels[i] =  nullptr;
                node_labels[i] = nullptr;
            }
        }
    }
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

                #define DEF_OP(name, num, sign, sign_len, plot_sign, tex_pre_sign, tex_left_cond, tex_mid_sign, tex_right_cond, tex_end_sign)   \
                case OP_##name:                                                                                                                 \
                {                                                                                                                               \
                    tex_print (tex_pre_sign);                                                                                                   \
                                                                                                                                                \
                    if (tex_left_cond)                                                                                                          \
                    {                                                                                                                           \
                        tex_print ("(");                                                                                                        \
                        tex_print_node (node->left, tex_file);                                                                                  \
                        tex_print (")");                                                                                                        \
                    }                                                                                                                           \
                    else                                                                                                                        \
                    {                                                                                                                           \
                        tex_print_node (node->left, tex_file);                                                                                  \
                    }                                                                                                                           \
                                                                                                                                                \
                    tex_print (tex_mid_sign);                                                                                                   \
                                                                                                                                                \
                    if (tex_right_cond)                                                                                                         \
                    {                                                                                                                           \
                        tex_print ("(");                                                                                                        \
                        tex_print_node (node->right, tex_file);                                                                                 \
                        tex_print (")");                                                                                                        \
                    }                                                                                                                           \
                    else                                                                                                                        \
                    {                                                                                                                           \
                        tex_print_node (node->right, tex_file);                                                                                 \
                    }                                                                                                                           \
                                                                                                                                                \
                    tex_print (tex_end_sign);                                                                                                   \
                                                                                                                                                \
                    break;                                                                                                                      \
                }

                #include "../../operations.h"

                default:
                {
                    printf ("Error: wrong node op type in %s", __PRETTY_FUNCTION__);
                    break;
                }

                #undef DEF_OP
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
                            "set samples 1000\n"
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

                #define DEF_OP(name, num, sign, sign_len, plot_sign,...)    \
                case OP_##name:                                             \
                {                                                           \
                    sprintf (graphic_func + shift, #plot_sign);             \
                    shift += sign_len;                                      \
                                                                            \
                    break;                                                  \
                }

                #include "../../operations.h"

                #undef DEF_OP

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
    tree_check (func, &err);

    tex_print ("\\[%s(x) = ", func_name);

    Tree compress_tree = {};

    if (tree_compression (func, &compress_tree) == nullptr)
    {
        tex_print_node (func->root, tex_file);
        tex_print ("\\]\\newline\n");
    }
    else
    {
        tree_check (&compress_tree, &err);
        tex_print_compressed_node (compress_tree.root, tex_file, func->root);
        tree_dtor (&compress_tree);
    }
}

void print_diff_func (const char *func_name, Tree *diff_func, FILE *tex_file, int diff_order)
{
    tree_check (diff_func, &err);


    if (diff_order == 1)
    {
        tex_print ("\\[%s'(x) = ", func_name);
    }
    else
    {
        tex_print ("\\[%s^{(%d)}(x) = ", func_name, diff_order);
    }

    Tree compress_tree = {};

    if (tree_compression (diff_func, &compress_tree) == nullptr)
    {
        tex_print_node (diff_func->root, tex_file);
        tex_print ("\\]\\newline\n");
    }
    else
    {
        tree_check (&compress_tree, &err);
        tex_print_compressed_node (compress_tree.root, tex_file, diff_func->root);
        tree_dtor (&compress_tree);
    }
}

void print_point_value (const char *func_name, double point, double point_value, FILE *tex_file)
{
    tex_print ("Найдем значение функции в точке %.2lf знаменитым в 17 веке методом буль-буль\\newline", point);

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
    tree_check (decompose_tree, &err);

    tex_print ("Разложим функцию (по Тейлору) до $o(x^{%d})$ в точке а = %.2lf\\newline\n", decompose_order, decompose_point);

    tex_print ("\\[f(a) = ");

    Tree compress_tree = {};

    if (tree_compression (decompose_tree, &compress_tree) == nullptr)
    {
        tex_print_node (decompose_tree->root, tex_file);
        tex_print ("\\]\\newline\n");
    }
    else
    {
        tex_print_compressed_node (compress_tree.root, tex_file, decompose_tree->root);
        tree_dtor (&compress_tree);
    }
}

void print_tex_ending (FILE *tex_file)
{
    fprintf (tex_file, "\\end {document}");
}

#undef debug_print
#undef tex_print
