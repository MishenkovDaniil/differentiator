#include <stdio.h>
#include <assert.h>

#include "tree/tree.h"
#include "tree/io/tree_input.h"
#include "tree/io/tree_output.h"
#include "tree/diff/tree_diff.h"

int main ()
{
    const char *input_file_name = "def_input.txt";
    const char *tex_file_name = "àààà_äèôôóðû.txt";

    FILE *input_file = fopen (input_file_name, "r");
    FILE *tex_file = fopen (tex_file_name, "w");

    assert (input_file);
    assert (tex_file);

    Tree tree = {};

    //tree_ctor (&tree);
    //tree.root = GetNodeG (&tree, "(x + 2) * (x - 3)");
    //unsigned int errr = 0;
    //unsigned int *err = &errr;
    //tree_check (&tree, err);


    //printf ("%d", tree.root->value.op_val);

    //printf ("1");
    tree_fill (&tree, input_file, input_file_name);

    //printf ("\n\n\n%d", tree.root->value.op_val);


    unsigned int errr = 0;
    unsigned int *err = &errr;
    tree_check (&tree, err);

    Tree def_tree = {};
    def_tree.root = tree_diff  (tree.root, tex_file);

    tree_check (&def_tree, err);

    print_tex_ending (tex_file);


    fclose (tex_file);
    fclose (input_file);
    tree_dtor (&def_tree);
    tree_dtor (&tree);
}
