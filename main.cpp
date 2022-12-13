#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "tree/tree.h"
#include "tree/io/tree_input.h"
#include "tree/io/tree_output.h"
#include "config.h"
#include "string.h"

// diff --input equation -o book.tex --taylor 4 3.21 --plot graph.png --eval 6.2342341235
// 1. Все опции должны быть как long так и short: --taylor -t
// 2. Must be --help
// 3. Способ легко добавить твои опции

// while (...) {
//    for (int i ...)
//        if (strcmp(input, options[i].string))
//            options[i].action(next token);
//}
// argv argc
// struct Option {
//    name = "asdf";
//    func *action;
//}

// getopt()

int main (int argc, const char **argv)
{
    const char *input_file_name   = "def_input.txt";
    const char *tex_file_name     = "AAAAAA_diffurs.tex";
    const char *graphic_file_name = "graphic.txt";
    const char *func_name         = "f";

    char *tex_to_pdf_cmd = (char *)calloc (strlen ("pdflatex ") + strlen (tex_file_name) + 1, sizeof (char));
    strcat (tex_to_pdf_cmd, "pdflatex ");
    strcat (tex_to_pdf_cmd, tex_file_name);
    strcat (tex_to_pdf_cmd, " > /dev/null");

    FILE *input_file   = fopen (input_file_name, "r");
    FILE *tex_file     = fopen (tex_file_name, "wb");
    FILE *graphic_file = fopen (graphic_file_name, "w");

    if (input_file == nullptr)
    {
        perror ("fopen of input file failed");
        return 0;
    }
    if (tex_file == nullptr)
    {
        perror ("fopen of tex file failed");
        return 0;
    }
    if (graphic_file == nullptr)
    {
        perror ("fopen of graphic file failed");
        return 0;
    }

    assert (input_file);
    assert (tex_file);
    assert (graphic_file);

    Config config = {};

    if (get_config (&config) == false)
    {
        return 0;
    }

    Tree expr = {};
    tree_fill (&expr, input_file, input_file_name);

    print_tex_header (tex_file);
    new_section (tex_file);

    tex_print_func (func_name, &expr, tex_file);

    tex_find_point_value (&expr, func_name, &config, tex_file);

    tex_find_diff (&expr, func_name, &config, tex_file);

    tex_decompose_func (&expr, func_name, graphic_file_name, input_file_name, &config, tex_file, graphic_file);

    print_tex_ending (tex_file);

    fflush (tex_file);
    system (tex_to_pdf_cmd);

    fclose (tex_file);
    fclose (input_file);
    fclose (graphic_file);

    free (tex_to_pdf_cmd);

    tree_dtor (&expr);
}

#undef tex_print
#undef debug_print
