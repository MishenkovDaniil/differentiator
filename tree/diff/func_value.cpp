#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "../tree.h"
#include "func_value.h"
#include "../io/tree_output.h"

double func_point_value (Tree *tree, double point)
{
    double result = node_point_value (tree->root, point);

    if (result == NAN)
    {
        printf ("function is undefined in point %d.\n", point);
    }

    return result;
}

double node_point_value (const Node *node, double point)
{
    switch (node->type)
    {
        case TYPE_NUM:
            return node->value.dbl_val;
        case TYPE_VAR:
            return point;
        case TYPE_OP:
        {
            #define Left                node->left
            #define Right               node->right
            #define func_lval           node_point_value (Left, point)
            #define func_rval           node_point_value (Right, point)
            ////#define OP(lval, rval, op)  (lval && rval) ? *lval sign *rval : nullptr
            #define ADD(lval, rval)     lval + rval
            #define SUB(lval, rval)     lval - rval
            #define MUL(lval, rval)     lval * rval
            #define DIV(lval, rval)     lval / rval
            #define DEG(lval, rval)     pow (lval, rval)
            #define SIN(rval)           sin (rval)
            #define COS(rval)           cos (rval)

            switch (node->value.op_val)
            {
                case OP_ADD:
                {
                    return ADD (func_lval, func_rval);
                }
                case OP_SUB:
                {
                    return SUB (func_lval, func_rval);
                }
                case OP_MUL:
                {
                    return MUL (func_lval, func_rval);
                }
                case OP_DIV:
                {
                    double divider = func_rval;

                    if (divider)
                    {
                        return DIV (func_lval, divider);
                    }
                    else
                    {
                        printf ("Error: division by zero in this point, func value is undefined");
                        return NAN;
                    }
                }
                case OP_DEG:
                {
                    return DEG (func_lval, func_rval);
                }
                case OP_SIN:
                {
                    return SIN (func_rval);
                }
                case OP_COS:
                {
                    return COS (func_rval);
                }
                default:
                {
                    debug_print ("Error: unknown operation %d", node->value.op_val);
                    return NAN;
                }
            }
            #undef Left
            #undef Right
            #undef lval
            #undef rval
            #undef ADD
            #undef SUB
            #undef MUL
            #undef DIV
            #undef DEG
            #undef SIN
            #undef COS
        }
        default:
        {
            debug_print ("Error: wrong node type %d", node->type);
            return NAN;
        }
    }
}

#undef tex_print
#undef debug_print
