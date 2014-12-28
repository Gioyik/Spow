#ifndef ZL_EVAL_H
#define ZL_EVAL_H

#define EVAL_ARGS(env, args) { \
    args = zlval_eval_args(env, args); \
    if (args->type == ZLVAL_ERR) { \
        return args; \
    } \
}

#define EVAL_SINGLE_ARG(env, args, i) { \
    args = zlval_eval_arg(env, args, i); \
    if (args->type == ZLVAL_ERR) { \
        return args; \
    } \
}

#include "types.h"

void zlval_eval_abort(void);

/* eval functions */
zlval* zlval_eval(zlenv* e, zlval* v);
zlval* zlval_eval_arg(zlenv* e, zlval* v, int arg);
zlval* zlval_eval_args(zlenv* e, zlval* v);
zlval* zlval_eval_sexpr(zlenv* e, zlval* v);
zlval* zlval_call(zlenv* e, zlval* f, zlval* a);
zlval* zlval_eval_macro(zlval* m);
zlval* zlval_eval_inside_qexpr(zlenv* e, zlval* v);
zlval* zlval_eval_cexpr(zlenv* e, zlval* v);

#endif
