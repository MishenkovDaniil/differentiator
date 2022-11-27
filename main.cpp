#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "tree/tree.h"
#include "tree/io/tree_input.h"
#include "tree/io/tree_output.h"
#include "tree/diff/tree_diff.h"
#include "tree/tree_convolution.h"

int main ()
{
    const char *input_file_name = "def_input.txt";
    const char *tex_file_name = "AAAAAA_diffurs.txt";

    unsigned int errr = 0;
    unsigned int *err = &errr;

    FILE *input_file = fopen (input_file_name, "r");
    FILE *tex_file = fopen (tex_file_name, "w");
    assert (input_file);
    assert (tex_file);

    Tree tree = {};
    tree_fill (&tree, input_file, input_file_name);

    Node *func_name = tree_create_node (TYPE_VAR, "y");

    print_tex_header (tex_file);

    fprintf (tex_file, "\\section{Глава I}\nФункция такова\\newline\n");
    tree_tex_print (func_name, tree.root, tex_file);
    fprintf(tex_file, "В упрощенном виде это будет выглядеть так:\\newline\n");

    tree_check (&tree, err);
    tree_convolution (tree.root);
    tree_check (&tree, err);
    tree_tex_print (func_name, tree.root, tex_file);

    printf ("Enter diff order to find: ");
    int diff_order = 0;
    scanf ("%d", &diff_order);

    fprintf (tex_file, "Найдем производную %d порядка этой функции методом Шварцвальда III\\newline\n", diff_order);

    if (diff_order <= 0)
    {
        printf ("Error: diff order must be above zero");
        return 0;
    }

    Tree def_tree = {};

    def_tree.root = find_diff (tree.root, tex_file, diff_order);
    tree_check (&def_tree, err);
    fprintf (tex_file, "Для простоты запишем этот результат так:\\newline\n");
    tree_convolution (def_tree.root);
    tree_check (&def_tree, err);
    printf ("[%d]\n", __LINE__);
    tree_tex_print (func_name, def_tree.root, tex_file, true);

    print_tex_ending (tex_file);


    fclose (tex_file);
    fclose (input_file);

    tree_dtor (&def_tree);
    tree_dtor (&tree);

    free (func_name);
}
