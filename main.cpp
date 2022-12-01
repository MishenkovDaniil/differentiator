#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "tree/tree.h"
#include "tree/io/tree_input.h"
#include "tree/io/tree_output.h"
#include "tree/diff/tree_diff.h"
#include "tree/tree_convolution.h"
#include "tree/diff/func_value.h"
#include "tree/diff/decomposition.h"

#define tex_print(...) fprintf (tex_file, __VA_ARGS__)

int main ()
{
    const char *input_file_name   = "def_input.txt";
    const char *tex_file_name     = "AAAAAA_diffurs.txt";
    const char *graphic_file_name = "graphic.txt";
    const char *func_name         = "f";

    unsigned int errr = 0;
    unsigned int *err = &errr;

    FILE *input_file   = fopen (input_file_name, "r");
    FILE *tex_file     = fopen (tex_file_name, "w");
    FILE *graphic_file = fopen (graphic_file_name, "w");
    assert (input_file);
    assert (tex_file);
    assert (graphic_file);

    Tree func = {};
    tree_fill (&func, input_file, input_file_name);

    print_tex_header (tex_file);
    new_section (tex_file);

//printing function

    print_original_function (func_name, &func, tex_file);

    tex_print ("Упростим функцию, чтобы с ней было более удобно работать.\\newline\n"
               "Получим следующий результат:\\newline\\newline\n");
    tree_convolution (func.root);

    print_function (func_name, &func, tex_file);

//printing and finding func value in point
    double point = 0;

    printf ("Enter point a to find f(a):");
    scanf ("%lf", &point);

    double func_value = func_point_value (&func, point);

    print_point_value (func_name, point, func_value, tex_file);

//printing and finding func diff
    int diff_order = 0;

    printf ("Enter diff order to find: ");
    scanf ("%d", &diff_order);

    tex_print ("Найдем производную %d порядка этой функции методом Шварцвальда III\\newline\n", diff_order);

    if (diff_order <= 0)
    {
        printf ("Error: diff order must be above zero");
        return 0;
    }

    Tree def_tree = {};

    def_tree.root = find_diff (func.root, tex_file, diff_order);
    tree_check (&def_tree, err);
    tex_print ("Для простоты запишем этот результат так:\\newline\n");
    tree_convolution (def_tree.root);

    print_diff_func (func_name, &def_tree, tex_file, diff_order);

//decomposing tree by Taylor and printing decomposition
    int decompose_order = 0;
    double decompose_point = 0;

    printf ("Enter decomposition order: ");
    scanf ("%d", &decompose_order);
    printf ("And enter point of decomposition: ");
    scanf ("%lf", &decompose_point);

    Tree decompose_tree = {};
    decompose (&decompose_tree, func.root, decompose_order, decompose_point);

    print_decompose_tree (func_name, &decompose_tree, decompose_point, decompose_order, tex_file);

    tex_print_graphic (graphic_file, graphic_file_name, input_file_name, &func, tex_file);

//generating tex ends
    print_tex_ending (tex_file);

//closing and freeing
    fclose (tex_file);
    fclose (input_file);
    fclose (graphic_file);

    tree_dtor (&decompose_tree);
    tree_dtor (&def_tree);
    tree_dtor (&func);
}

#undef tex_print
