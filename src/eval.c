#include "../include/eval.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/builtins.h"
#include "../include/util.h"

#define ZLENV_DEL_RECURSING(e) { \
    if (recursing) { \
        zlenv_del(e); \
    } \
}

static bool eval_aborted = false;

void zlval_eval_abort(void) {
    eval_aborted = true;
}

zlval* zlval_eval(zlenv* e, zlval* v) {
    bool recursing = false;

    while (true) {
        // Handle abort
        if (eval_aborted) {
            ZLENV_DEL_RECURSING(e);
            zlval_del(v);

            eval_aborted = false;
            return zlval_err("eval aborted");
        }

        switch (v->type) {
            case ZLVAL_SYM:
            {
                zlval* x = zlenv_get(e, v);
                zlval_del(v);
                ZLENV_DEL_RECURSING(e);
                return x;
                break;
            }

            case ZLVAL_SEXPR:
            {
                zlval* x = zlval_eval_sexpr(e, v);

                /* recursively evaluate results */
                if (x->type == ZLVAL_FN && x->called) {
                    ZLENV_DEL_RECURSING(e);
                    recursing = true;

                    e = zlenv_copy(x->env);
                    v = zlval_copy(x->body);

                    zlval_del(x);
                } else {
                    /* evaluate result in next loop */
                    v = x;
                }
                break;
            }

            case ZLVAL_QEXPR:
            {
                zlval* x = zlval_eval_inside_qexpr(e, v);
                ZLENV_DEL_RECURSING(e);
                return x;
                break;
            }

            case ZLVAL_EEXPR:
            case ZLVAL_CEXPR:
            {
                ZLENV_DEL_RECURSING(e);
                zlval* err = zlval_err("cannot directly evaluate %s; must be contained inside %s",
                        zlval_type_name(v->type), zlval_type_name(ZLVAL_QEXPR));
                zlval_del(v);
                return err;
            }

            default:
                ZLENV_DEL_RECURSING(e);
                return v;
                break;
        }
    }
}

zlval* zlval_eval_arg(zlenv* e, zlval* v, int arg) {
    v->cell[arg] = zlval_eval(e, v->cell[arg]);
    if (v->cell[arg]->type == ZLVAL_ERR) {
        return zlval_take(v, arg);
    }
    return v;
}

zlval* zlval_eval_args(zlenv* e, zlval* v) {
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = zlval_eval(e, v->cell[i]);
    }

    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == ZLVAL_ERR) {
            return zlval_take(v, i);
        }
    }
    return v;
}

zlval* zlval_eval_sexpr(zlenv* e, zlval* v) {
    if (v->count == 0) {
        zlval_del(v);
        return zlval_err("cannot evaluate empty %s", zlval_type_name(ZLVAL_SEXPR));
    }

    EVAL_SINGLE_ARG(e, v, 0);
    zlval* f = zlval_pop(v, 0);

    if (!ISCALLABLE(f->type)) {
        zlval* err = zlval_err("cannot evaluate %s; incorrect type for arg 0; got %s, expected callable",
                zlval_type_name(ZLVAL_SEXPR), zlval_type_name(f->type));
        zlval_del(v);
        zlval_del(f);
        return err;
    }

    zlval* result = zlval_call(e, f, v);
    zlval_del(f);
    return result;
}

zlval* zlval_call(zlenv* e, zlval* f, zlval* a) {
    /* calls a function if builtin, or evals a macro, else fills in the
     * corresponding parameters, and lets zlval_eval perform tail
     * call optimization
     */
    if (f->type == ZLVAL_BUILTIN) {
        return f->builtin(e, a);
    }

    int given = a->count;
    int total = f->formals->count;

    /* special case for macros */
    if (f->type == ZLVAL_MACRO) {
        for (int i = 0; i < a->count; i++) {
            /* wrap SExprs and Symbols in QExprs to avoid evaluation */
            if (a->cell[i]->type == ZLVAL_SEXPR) {
                zlval* q = zlval_qexpr();
                q = zlval_add(q, a->cell[i]);
                a->cell[i] = q;
            } else if (a->cell[i]->type == ZLVAL_SYM) {
                zlval* q = zlval_qexpr();
                q = zlval_add(q, a->cell[i]);
                a->cell[i] = q;
            }
        }
    }

    while (a->count) {
        if (f->formals->count == 0) {
            zlval_del(a);
            return zlval_err("%s passed too many arguments; got %i, expected %i",
                    zlval_type_name(f->type), given, total);
        }
        zlval* sym = zlval_pop(f->formals, 0);

        /* special case for variadic functions */
        if (streq(sym->sym, "&")) {
            if (f->formals->count != 1) {
                zlval_del(a);
                return zlval_err("function format invalid; symbol '&' not followed by single symbol");
            }

            zlval* nsym = zlval_pop(f->formals, 0);
            zlval* varargs = builtin_list(e, a);

            if (varargs->type == ZLVAL_ERR) {
                zlval_del(sym);
                zlval_del(nsym);
                return varargs;
            }

            zlenv_put(f->env, nsym, varargs);
            zlval_del(sym);
            zlval_del(nsym);
            break;
        }

        zlval* val = zlval_eval(e, zlval_pop(a, 0));
        if (val->type == ZLVAL_ERR) {
            zlval_del(sym);
            zlval_del(a);
            return val;
        }

        zlenv_put(f->env, sym, val);
        zlval_del(sym);
        zlval_del(val);
    }

    /* Special case for pure variadic function with no arguments */
    if (f->formals->count > 0 &&
            streq(f->formals->cell[0]->sym, "&")) {
        if (f->formals->count != 2) {
            return zlval_err("function format invalid; symbol '&' not followed by single symbol");
        }
        zlval_del(zlval_pop(f->formals, 0));
        zlval* sym = zlval_pop(f->formals, 0);
        zlval* val = zlval_qexpr();

        zlenv_put(f->env, sym, val);
        zlval_del(sym);
        zlval_del(val);
    }

    if (f->formals->count == 0) {
        f->called = true;
    }

    zlval_del(a);

    /* Handle macros -- they are called directly because their output must
     * be evaluated in the enclosing environment */
    if (f->type == ZLVAL_MACRO && f->called) {
        return zlval_eval_macro(f);
    } else {
        return zlval_copy(f);
    }
}

zlval* zlval_eval_macro(zlval* m) {
    zlenv* e = zlenv_copy(m->env);
    zlval* b = zlval_copy(m->body);

    zlval* v = zlval_eval(e, b);

    zlenv_del(e);

    if (v->type == ZLVAL_QEXPR) {
        v->type = ZLVAL_SEXPR;
    }
    return v;
}

zlval* zlval_eval_inside_qexpr(zlenv* e, zlval* v) {
    switch (v->type) {
        case ZLVAL_SEXPR:
        case ZLVAL_QEXPR:
        {
            for (int i = 0; i < v->count; i++) {
                // Special case for C-Expressions
                if (v->cell[i]->type == ZLVAL_CEXPR) {
                    zlval* cexpr = zlval_eval_cexpr(e, zlval_pop(v, i));
                    if (cexpr->type == ZLVAL_ERR) {
                        zlval_del(v);
                        return cexpr;
                    }

                    // Populate current container with result of CExpr
                    if (cexpr->type == ZLVAL_QEXPR) {
                        v = zlval_shift(v, cexpr, i);
                    } else {
                        v = zlval_insert(v, cexpr, i);
                    }
                } else {
                    v->cell[i] = zlval_eval_inside_qexpr(e, v->cell[i]);
                    if (v->cell[i]->type == ZLVAL_ERR) {
                        return zlval_take(v, i);
                    }
                }
            }
            return v;
            break;
        }

        case ZLVAL_EEXPR:
        {
            return zlval_eval(e, zlval_take(v, 0));
            break;

        }

        default:
            return v;
            break;
    }
}

zlval* zlval_eval_cexpr(zlenv* e, zlval* v) {
    zlval* res = zlval_eval(e, zlval_take(v, 0));
    if (res->type == ZLVAL_ERR) {
        return res;
    }
    return res;
}
