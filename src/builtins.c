#include "../include/builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#include "../include/assert.h"
#include "../include/eval.h"
#include "../include/parser.h"
#include "../include/print.h"
#include "../include/repl.h"
#include "../include/util.h"

#define UNARY_OP(a, op) { \
    switch (a->type) { \
        case ZLVAL_INT: \
            a->lng = op a->lng; \
            break; \
        case ZLVAL_FLOAT: \
            a->dbl = op a->dbl; \
            break; \
        default: break; \
    } \
}

#define BINARY_OP(a, b, op) { \
    switch (a->type) { \
        case ZLVAL_INT: \
            a->lng = a->lng op b->lng; \
            break; \
        case ZLVAL_FLOAT: \
            a->dbl = a->dbl op b->dbl; \
            break; \
        default: break; \
    } \
}

#define BINARY_OP_RES(res, a, b, op) { \
    switch (a->type) { \
        case ZLVAL_INT: \
            res = a->lng op b->lng; \
            break; \
        case ZLVAL_FLOAT: \
            res = a->dbl op b->dbl; \
            break; \
        default: break; \
    } \
}

#define BINARY_OP_FUNC(a, b, func) { \
    switch (a->type) { \
        case ZLVAL_INT: \
            a->lng = func (a->lng, b->lng); \
            break; \
        case ZLVAL_FLOAT: \
            a->dbl = func (a->dbl, b->dbl); \
            break; \
        default: break; \
    } \
}

long modulo(long x, long y) {
    /* Modulo operator, always positive */
    return labs(x) % labs(y);
}

double fmodulo(double x, double y) {
    /* Modulo operator, always positive */
    return fmod(fabs(x), fabs(y));
}

zlval* builtin_num_op(zlenv* e, zlval* a, char* op) {
    /* Argcount must be checked in calling function, because
     * different operators have different requirements */
    EVAL_ARGS(e, a);

    for (int i = 0; i < a->count; i++) {
        ZLASSERT_ISNUMERIC(a, i, op);
    }

    zlval* x = zlval_pop(a, 0);
    if (streq(op, "-") && a->count == 0) {
        UNARY_OP(x, -);
    }

    while (a->count > 0) {
        zlval* y = zlval_pop(a, 0);

        zlval_maybe_promote_numeric(x, y);

        if (streq(op, "+")) { BINARY_OP(x, y, +); }
        if (streq(op, "-")) { BINARY_OP(x, y, -); }
        if (streq(op, "*")) { BINARY_OP(x, y, *); }
        if (streq(op, "/") || streq(op, "//") || streq(op, "%")) {
            /* Handle division or modulo by zero */
            if ((y->type == ZLVAL_INT && y->lng == 0) ||
                (y->type == ZLVAL_FLOAT && y->dbl == 0)) {
                zlval* err = zlval_err("division by zero; %i %s 0", x->lng, op);
                zlval_del(x);
                zlval_del(y);
                x = err;
                break;
            }

            if (strstr(op, "/")) {
                zlval_type_t oldtype_x = x->type;
                zlval_type_t oldtype_y = y->type;

                /* Handle fractional integer division */
                if (x->type == ZLVAL_INT && y->type == ZLVAL_INT && x->lng % y->lng != 0) {
                    zlval_promote_numeric(x);
                    zlval_promote_numeric(y);
                }

                BINARY_OP(x, y, /);

                if (streq(op, "//")) {
                    /* Truncating division, based on old types */
                    if (oldtype_x == ZLVAL_INT && oldtype_y == ZLVAL_INT) {
                        zlval_demote_numeric(x);
                    } else {
                        /* Truncate, but we still need to keep result a float */
                        zlval_demote_numeric(x);
                        zlval_promote_numeric(x);
                    }
                }
            } else {
                /* Handle modulo */
                if (x->type == ZLVAL_FLOAT || y->type == ZLVAL_FLOAT) {
                    BINARY_OP_FUNC(x, y, fmodulo);
                } else {
                    x->lng = modulo(x->lng, y->lng);
                }
            }
        }
        if (streq(op, "^")) {
            if (x->type == ZLVAL_FLOAT || y->type == ZLVAL_FLOAT) {
                BINARY_OP_FUNC(x, y, pow);
            } else {
                /* Handle case where result is fractional */
                double ans = pow((double)x->lng, y->lng);
                if (ans - (double)(long)ans != 0.0) {
                    x->type = ZLVAL_FLOAT;
                    x->dbl = ans;
                } else {
                    x->lng = (long)ans;
                }
            }

            // Check for NaN
            if (x->type == ZLVAL_FLOAT && isnan(x->dbl)) {
                zlval_del(x);
                x = zlval_err("pow resulted in NaN");
            }
        }

        zlval_del(y);
    }

    zlval_del(a);
    return x;
}

zlval* builtin_add(zlenv* e, zlval* a) {
    ZLASSERT_MINARGCOUNT(a, 2, "+");
    return builtin_num_op(e, a, "+");
}

zlval* builtin_sub(zlenv* e, zlval* a) {
    ZLASSERT_MINARGCOUNT(a, 1, "-");
    return builtin_num_op(e, a, "-");
}

zlval* builtin_mul(zlenv* e, zlval* a) {
    ZLASSERT_MINARGCOUNT(a, 2, "*");
    return builtin_num_op(e, a, "*");
}

zlval* builtin_div(zlenv* e, zlval* a) {
    ZLASSERT_MINARGCOUNT(a, 2, "/");
    return builtin_num_op(e, a, "/");
}

zlval* builtin_trunc_div(zlenv* e, zlval* a) {
    ZLASSERT_MINARGCOUNT(a, 2, "//");
    return builtin_num_op(e, a, "//");
}

zlval* builtin_mod(zlenv* e, zlval* a) {
    ZLASSERT_MINARGCOUNT(a, 2, "%");
    return builtin_num_op(e, a, "%");
}

zlval* builtin_pow(zlenv* e, zlval* a) {
    ZLASSERT_MINARGCOUNT(a, 2, "^");
    return builtin_num_op(e, a, "^");
}

zlval* builtin_ord_op(zlenv* e, zlval* a, char* op) {
    ZLASSERT_ARGCOUNT(a, 2, op);
    EVAL_ARGS(e, a);
    ZLASSERT_ISNUMERIC(a, 0, op);
    ZLASSERT_ISNUMERIC(a, 1, op);

    zlval* x = zlval_pop(a, 0);
    zlval* y = zlval_pop(a, 0);

    zlval_maybe_promote_numeric(x, y);

    bool res;
    if (streq(op, ">")) {
        BINARY_OP_RES(res, x, y, >);
    }
    if (streq(op, "<")) {
        BINARY_OP_RES(res, x, y, <);
    }
    if (streq(op, ">=")) {
        BINARY_OP_RES(res, x, y, >=);
    }
    if (streq(op, "<=")) {
        BINARY_OP_RES(res, x, y, <=);
    }

    zlval_del(y);
    zlval_del(a);

    x->type = ZLVAL_BOOL;
    x->bln = res;
    return x;
}

zlval* builtin_gt(zlenv* e, zlval* a) {
    return builtin_ord_op(e, a, ">");
}

zlval* builtin_gte(zlenv* e, zlval* a) {
    return builtin_ord_op(e, a, ">=");
}

zlval* builtin_lt(zlenv* e, zlval* a) {
    return builtin_ord_op(e, a, "<");
}

zlval* builtin_lte(zlenv* e, zlval* a) {
    return builtin_ord_op(e, a, "<=");
}

zlval* builtin_logic_op(zlenv* e, zlval* a, char* op) {
    ZLASSERT_ARGCOUNT(a, 2, op);
    EVAL_ARGS(e, a);

    bool eq = zlval_eq(a->cell[0], a->cell[1]);
    bool res;
    if (streq(op, "==")) {
        res = eq;
    }
    if (streq(op, "!=")) {
        res = !eq;
    }
    zlval_del(a);
    return zlval_bool(res);
}

zlval* builtin_eq(zlenv* e, zlval* a) {
    return builtin_logic_op(e, a, "==");
}

zlval* builtin_neq(zlenv* e, zlval* a) {
    return builtin_logic_op(e, a, "!=");
}

zlval* builtin_bool_op(zlenv* e, zlval* a, char* op) {
    if (streq(op, "not")) {
        ZLASSERT_ARGCOUNT(a, 1, op);
        EVAL_SINGLE_ARG(e, a, 0);
        ZLASSERT_TYPE(a, 0, ZLVAL_BOOL, op);

        zlval* x = zlval_take(a, 0);
        x->bln = !x->bln;
        return x;
    }

    ZLASSERT_ARGCOUNT(a, 2, op);

    zlval* x = zlval_pop(a, 0);
    zlval* y = zlval_take(a, 0);

    x = zlval_eval(e, x);
    if (x->type != ZLVAL_BOOL) {
        zlval* err = zlval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 0, zlval_type_name(x->type), zlval_type_name(ZLVAL_BOOL));
        zlval_del(x);
        zlval_del(y);
        return err;
    }

    if ((streq(op, "and") && !x->bln) || (streq(op, "or") && x->bln)) {
        zlval_del(y);
        return x;
    }

    y = zlval_eval(e, y);
    if (y->type != ZLVAL_BOOL) {
        zlval* err = zlval_err(
                "function '%s' passed incorrect type for arg %i; got %s, expected %s",
                op, 1, zlval_type_name(y->type), zlval_type_name(ZLVAL_BOOL));
        zlval_del(x);
        zlval_del(y);
        return err;
    }

    if (streq(op, "and")) {
        x->bln = x->bln && y->bln;
    }
    if (streq(op, "or")) {
        x->bln = x->bln || y->bln;
    }

    zlval_del(y);
    return x;
}

zlval* builtin_and(zlenv* e, zlval* a) {
    return builtin_bool_op(e, a, "and");
}

zlval* builtin_or(zlenv* e, zlval* a) {
    return builtin_bool_op(e, a, "or");
}

zlval* builtin_not(zlenv* e, zlval* a) {
    return builtin_bool_op(e, a, "not");
}

zlval* builtin_head(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "head");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_QEXPR, "head");
    ZLASSERT_NONEMPTY(a, a->cell[0], "head");

    zlval* v = zlval_take(a, 0);
    return zlval_eval(e, zlval_take(v, 0));
}

zlval* builtin_qhead(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "qhead");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_QEXPR, "qhead");
    ZLASSERT_NONEMPTY(a, a->cell[0], "qhead");

    zlval* q = zlval_take(a, 0);
    zlval* v = zlval_take(q, 0);

    if (v->type == ZLVAL_SEXPR) {
        v->type = ZLVAL_QEXPR;
    } else if (v->type == ZLVAL_SYM) {
        v->type = ZLVAL_QSYM;
    }
    return zlval_eval(e, v);
}

zlval* builtin_tail(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "tail");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_QEXPR, "tail");
    ZLASSERT_NONEMPTY(a, a->cell[0], "tail");

    zlval* v = zlval_take(a, 0);
    return zlval_slice(v, 1, v->count);
}

zlval* builtin_first(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "first");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_QEXPR, "first");
    ZLASSERT_NONEMPTY(a, a->cell[0], "first");

    zlval* v = zlval_take(a, 0);
    return zlval_slice(v, 0, 1);
}

zlval* builtin_last(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "last");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_QEXPR, "last");
    ZLASSERT_NONEMPTY(a, a->cell[0], "last");

    zlval* v = zlval_take(a, 0);
    return zlval_slice(v, v->count - 1, v->count);
}

zlval* builtin_exceptlast(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "except-last");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_QEXPR, "except-last");
    ZLASSERT_NONEMPTY(a, a->cell[0], "except-last");

    zlval* v = zlval_take(a, 0);
    return zlval_slice(v, 0, v->count - 1);
}

zlval* builtin_list(zlenv* e, zlval* a) {
    EVAL_ARGS(e, a);

    a->type = ZLVAL_QEXPR;
    return a;
}

zlval* builtin_eval(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "eval");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_QEXPR, "eval");

    zlval* x = zlval_take(a, 0);
    x->type = ZLVAL_SEXPR;
    return zlval_eval(e, x);
}

zlval* builtin_append(zlenv* e, zlval* a) {
    ZLASSERT_MINARGCOUNT(a, 2, "append");
    EVAL_ARGS(e, a);

    for (int i = 0; i < a->count; i++) {
        ZLASSERT_TYPE(a, i, ZLVAL_QEXPR, "append");
    }

    zlval* x = zlval_pop(a, 0);
    while (a->count) {
        x = zlval_join(x, zlval_pop(a, 0));
    }

    zlval_del(a);
    return x;
}

zlval* builtin_cons(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 2, "cons");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 1, ZLVAL_QEXPR, "cons");

    zlval* v = zlval_pop(a, 0);
    zlval* x = zlval_take(a, 0);
    zlval_add_front(x, v);
    return x;
}

zlval* builtin_dictget(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 2, "dict-get");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_DICT, "dict-get");
    ZLASSERT_TYPE(a, 1, ZLVAL_QSYM, "dict-get");

    zlval* d = zlval_pop(a, 0);
    zlval* k = zlval_take(a, 0);

    zlval* v = zlval_get_dict(d, k);
    zlval_del(d);
    zlval_del(k);
    return v;
}

zlval* builtin_dictset(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 3, "dict-set");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_DICT, "dict-set");
    ZLASSERT_TYPE(a, 1, ZLVAL_QSYM, "dict-set");

    zlval* d = zlval_pop(a, 0);
    zlval* k = zlval_pop(a, 0);
    zlval* v = zlval_take(a, 0);

    d = zlval_add_dict(d, k, v);
    zlval_del(k);
    zlval_del(v);
    return d;
}

zlval* builtin_dictdel(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 2, "dict-del");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_DICT, "dict-del");
    ZLASSERT_TYPE(a, 1, ZLVAL_QSYM, "dict-del");

    zlval* d = zlval_pop(a, 0);
    zlval* k = zlval_take(a, 0);

    zlval_rm_dict(d, k);
    zlval_del(k);
    return d;
}

zlval* builtin_dicthaskey(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 2, "dict-haskey?");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_DICT, "dict-haskey?");
    ZLASSERT_TYPE(a, 1, ZLVAL_QSYM, "dict-haskey?");

    zlval* d = zlval_pop(a, 0);
    zlval* k = zlval_take(a, 0);

    bool v = zlval_haskey_dict(d, k);
    zlval_del(d);
    zlval_del(k);
    return zlval_bool(v);
}

zlval* builtin_dictkeys(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "dict-keys");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_DICT, "dict-keys");

    zlval* d = zlval_take(a, 0);

    zlval* v = zlval_keys_dict(d);
    zlval_del(d);
    return v;
}

zlval* builtin_dictvals(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "dict-vals");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_DICT, "dict-vals");

    zlval* d = zlval_take(a, 0);

    zlval* v = zlval_vals_dict(d);
    zlval_del(d);
    return v;
}

zlval* builtin_len(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "len");
    EVAL_ARGS(e, a);
    ZLASSERT_ISCOLLECTION(a, 0, "len");

    zlval* x = zlval_int(a->cell[0]->length);
    zlval_del(a);
    return x;
}

zlval* builtin_reverse(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "reverse");
    EVAL_ARGS(e, a);
    ZLASSERT_ISORDEREDCOLLECTION(a, 0, "reverse");

    zlval* collection = zlval_take(a, 0);

    return zlval_reverse(collection);
}

zlval* builtin_slice(zlenv* e, zlval* a) {
    ZLASSERT_RANGEARGCOUNT(a, 2, 4, "slice");
    EVAL_ARGS(e, a);
    ZLASSERT_ISORDEREDCOLLECTION(a, 0, "slice");
    ZLASSERT_TYPE(a, 1, ZLVAL_INT, "slice");

    bool end_arg_given = a->count > 2;
    if (end_arg_given) {
        ZLASSERT_TYPE(a, 2, ZLVAL_INT, "slice");
    }

    bool step_arg_given = a->count > 3;
    if (step_arg_given) {
        ZLASSERT_TYPE(a, 3, ZLVAL_INT, "slice");
        ZLASSERT_NONZERO(a, (int)a->cell[3]->lng, "slice");
    }

    zlval* collection = zlval_pop(a, 0);

    int start, end, step;
    bool reverse_slice = false;

    /* TODO: Index cast is unsafe here */
    start = (int)a->cell[0]->lng;
    end = end_arg_given ? (int)a->cell[1]->lng : collection->length;
    step = step_arg_given ? (int)a->cell[2]->lng : 1;

    zlval_del(a);

    /* Support negative indices to represent index from end */
    if (start < 0) {
        start = collection->length + start;
    }
    if (end < 0) {
        end = collection->length + end;
    }

    /* Handle negative step */
    if (step < 0) {
        step = -step;
        int temp = start;
        start = end;
        end = temp;
    }

    if (end < start) {
        reverse_slice = true;
        int temp = start + 1;
        start = end + 1;
        end = temp;
    }

    /* Constrain to collection bounds */
    start = start < 0 ? 0 :
        (start > collection->length ? collection->length : start);
    end = end < 0 ? 0 :
        (end > collection->length ? collection->length : end);

    collection = zlval_slice_step(collection, start, end, step);
    return reverse_slice ? zlval_reverse(collection) : collection;
}

zlval* builtin_if(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 3, "if");

    EVAL_SINGLE_ARG(e, a, 0);
    ZLASSERT_TYPE(a, 0, ZLVAL_BOOL, "if");

    zlval* x;
    /* does not eval result; sends it back to zlval_eval for
     * potential tail call optimization */
    if (a->cell[0]->bln) {
        x = zlval_pop(a, 1);
    } else {
        x = zlval_pop(a, 2);
    }

    zlval_del(a);
    return x;
}

zlval* builtin_var(zlenv* e, zlval* a, bool global) {
    char* op = global ? "var" : "local";
    ZLASSERT_MINARGCOUNT(a, 2, op);

    /* Special case when there is a single symbol to be defined */
    if (a->cell[0]->type == ZLVAL_SYM) {
        ZLASSERT_ARGCOUNT(a, 2, op);

        int index = zlenv_index(e, a->cell[0]);
        ZLASSERT(a, index == -1,
                "cannot redefine '%s'", a->cell[0]->sym);

        EVAL_SINGLE_ARG(e, a, 1);

        if (global) {
            zlenv_put_global(e, a->cell[0], a->cell[1]);
        } else {
            zlenv_put(e, a->cell[0], a->cell[1]);
        }
        return zlval_take(a, 1);
    }

    ZLASSERT_TYPE(a, 0, ZLVAL_SEXPR, op);

    zlval* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        ZLASSERT(a, (syms->cell[i]->type == ZLVAL_SYM),
                "function '%s' cannot define non-symbol at position %i", op, i);
    }

    for (int i = 0; i < syms->count; i++) {
        int index = zlenv_index(e, syms->cell[i]);
        ZLASSERT(a, index == -1,
                "cannot redefine '%s'", syms->cell[i]->sym);
    }

    ZLASSERT(a, (syms->count == a->count - 1),
            "function '%s' given non-matching number of symbols and values; %i symbols, %i values",
            op, syms->count, a->count - 1);

    /* Evaluate value arguments (but not the symbols) */
    for (int i = 1; i < a->count; i++) {
        EVAL_SINGLE_ARG(e, a, i);
    }

    for (int i = 0; i < syms->count; i++) {
        if (global) {
            zlenv_put_global(e, syms->cell[i], a->cell[i + 1]);
        } else {
            zlenv_put(e, syms->cell[i], a->cell[i + 1]);
        }
    }

    return zlval_take(a, a->count - 1);
}

zlval* builtin_define(zlenv* e, zlval* a) {
    return builtin_var(e, a, false);
}

zlval* builtin_global(zlenv* e, zlval* a) {
    return builtin_var(e, a, true);
}

zlval* builtin_let(zlenv* e, zlval* a) {
    /* TODO: maybe fix DEFINE and GLOBAL to work like this too... */
    ZLASSERT_ARGCOUNT(a, 2, "let");
    ZLASSERT_TYPE(a, 0, ZLVAL_SEXPR, "let");

    zlval* bindings = a->cell[0];
    /* verify structure of inner bindings list */
    for (int i = 0; i < bindings->count; i++) {
        ZLASSERT(a, (bindings->cell[i]->type == ZLVAL_SEXPR),
                "function '%s' requires %s in binding list at position %i",
                "let", zlval_type_name(ZLVAL_SEXPR), i);

        ZLASSERT(a, (bindings->cell[i]->count == 2 && bindings->cell[i]->cell[0]->type == ZLVAL_SYM),
                "function '%s' bindings must have two parts: %s and value, at position %i",
                "let", zlval_type_name(ZLVAL_SYM), i);
    }

    for (int i = 0; i < bindings->count; i++) {
        int index = zlenv_index(e, bindings->cell[i]->cell[0]);
        ZLASSERT(a, index == -1,
                "cannot redefine '%s'", bindings->cell[i]->cell[0]->sym);
    }

    // TODO: Because of reference cycles, this causes garbage to build up
    // Need to add GC
    zlenv* lenv = zlenv_new();
    lenv->parent = e;
    lenv->parent->references++;

    /* Evaluate value arguments (but not the symbols) */
    for (int i = 0; i < bindings->count; i++) {
        bindings->cell[i] = zlval_eval_arg(lenv, bindings->cell[i], 1);

        if (bindings->cell[i]->type == ZLVAL_ERR) {
            zlval* err = zlval_pop(bindings, i);
            zlval_del(a);
            lenv->parent = NULL;
            zlenv_del(lenv);
            return err;
        }

        zlval* sym = bindings->cell[i]->cell[0];
        zlval* val = bindings->cell[i]->cell[1];

        zlenv_put(lenv, sym, val);
    }

    zlval* v = zlval_eval(lenv, zlval_take(a, 1));

    zlenv_del(lenv);
    return v;
}

zlval* builtin_lambda(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 2, "fn");
    ZLASSERT_ISEXPR(a, 0, "fn");

    for (int i = 0; i < a->cell[0]->count; i++) {
        ZLASSERT(a, (a->cell[0]->cell[i]->type == ZLVAL_SYM),
                "function 'fn' cannot take non-symbol argument at position %i", i);
    }

    zlval* formals = zlval_pop(a, 0);
    zlval* body = zlval_take(a, 0);
    return zlval_lambda(e, formals, body);
}

zlval* builtin_macro(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 3, "macro");
    ZLASSERT_TYPE(a, 0, ZLVAL_SYM, "macro");
    ZLASSERT_ISEXPR(a, 1, "macro");

    int index = zlenv_index(e, a->cell[0]);
    ZLASSERT(a, index == -1,
            "cannot redefine '%s'", a->cell[0]->sym);

    for (int i = 0; i < a->cell[1]->count; i++) {
        ZLASSERT(a, (a->cell[1]->cell[i]->type == ZLVAL_SYM),
                "macro cannot take non-symbol argument at position %i", i);
    }

    zlval* name = zlval_pop(a, 0);
    zlval* formals = zlval_pop(a, 0);
    zlval* body = zlval_take(a, 0);

    zlval* macro = zlval_macro(e, formals, body);

    zlenv_put(e, name, macro);

    zlval_del(name);
    zlval_del(macro);
    return zlval_qexpr();
}

zlval* builtin_typeof(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "typeof");
    EVAL_ARGS(e, a);

    zlval* arg = zlval_take(a, 0);
    zlval* res = zlval_qsym(zlval_type_sysname(arg->type));
    zlval_del(arg);
    return res;
}

zlval* builtin_convert(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 2, "convert");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_QSYM, "convert");

    zlval* tsym = zlval_pop(a, 0);
    zlval* v = zlval_take(a, 0);

    zlval* res;
    zlval_type_t type = zlval_parse_sysname(tsym->sym);
    if (errno != EINVAL) {
        res = zlval_convert(type, v);
    } else {
        res = zlval_err("no such type: %s", tsym->sym);
    }

    zlval_del(tsym);
    zlval_del(v);

    return res;
}

zlval* builtin_import(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "import");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_STR, "import");

    // Check the import path
    char* importPath = safe_malloc(strlen(a->cell[0]->str) + 5); // extra space for extension
    strcpy(importPath, a->cell[0]->str);
    strcat(importPath, ".spow");

    // Attempt twice: once with the .spow extension, and once with the raw path
    for (int attempt = 0; attempt < 2; attempt++) {
        struct stat s;
        errno = 0;
        int statErr = stat(importPath, &s);

        bool hasError = statErr || !S_ISREG(s.st_mode);

        // Keep going if we've successfully found a file
        if (!hasError) {
            break;
        } else {
            free(importPath);

            // Try the raw path if we have once more attempt
            if (attempt == 0) {
                importPath = safe_malloc(strlen(a->cell[0]->str) + 1);
                strcpy(importPath, a->cell[0]->str);
            } else {
                // Return error otherwise
                zlval* errval;
                if (statErr && errno == ENOENT) {
                    errval = zlval_err("path '%s' does not exist", a->cell[0]->str);
                } else if (!S_ISREG(s.st_mode)) {
                    errval = zlval_err("path '%s' is not a regular file", a->cell[0]->str);
                } else {
                    errval = zlval_err("unknown import error");
                }
                zlval_del(a);
                return errval;
            }
        }
    }

    zlval* v;
    char* err;
    if (zlval_parse_file(importPath, &v, &err)) {
        free(importPath);

        while (v->count) {
            zlval* x = zlval_eval(e, zlval_pop(v, 0));
            if (x->type == ZLVAL_ERR) {
                zlval_println(x);
            }
            zlval_del(x);
        }

        zlval_del(v);
        zlval_del(a);

        return zlval_qexpr();
    } else {
        free(importPath);

        zlval* errval = zlval_err("could not import %s", err);
        free(err);
        zlval_del(a);

        return errval;
    }
}

zlval* builtin_print(zlenv* e, zlval* a) {
    EVAL_ARGS(e, a);
    for (int i = 0; i < a->count; i++) {
        if (i != 0) {
            zl_printf(" ");
        }
        if (a->cell[i]->type == ZLVAL_STR) {
            zl_printf("%s", a->cell[i]->str);
        } else {
            zlval_print(a->cell[i]);
        }
    }
    zlval_del(a);
    return zlval_qexpr();
}

zlval* builtin_println(zlenv* e, zlval* a) {
    zlval* x = builtin_print(e, a);
    zl_printf("\n");
    return x;
}

zlval* builtin_random(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 0, "random");
    double r = (double)rand() / (double)RAND_MAX;
    return zlval_float(r);
}

zlval* builtin_error(zlenv* e, zlval* a) {
    ZLASSERT_ARGCOUNT(a, 1, "error");
    EVAL_ARGS(e, a);
    ZLASSERT_TYPE(a, 0, ZLVAL_STR, "error");

    zlval* err = zlval_err(a->cell[0]->str);
    zlval_del(a);
    return err;
}

zlval* builtin_exit(zlenv* e, zlval* a) {
    zlval_del(a);
    abort_repl();
    return zlval_qexpr();
}
