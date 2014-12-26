#include "../include/builtins.h"

lval* builtin_head(lenv* e, lval* a)
{
    /* Check error conditions */
    LASSERT_NUM("head", a, 1);
    LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("head", a, 0);

    lval* v = lval_take(a, 0);

    while (v->count > 1) {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval* builtin_tail(lenv* e, lval* a)
{
    /* check error conditions */
    LASSERT_NUM("tail", a, 1);
    LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("tail", a, 0);

    lval* v = lval_take(a, 0);

    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_cons(lenv* e, lval* a)
{
    /* check error conditions */
    LASSERT_NUM("cons", a, 2);
    LASSERT_TYPE("cons", a, 0, LVAL_NUM);
    LASSERT_TYPE("cons", a, 1, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("cons", a, 0);

    lval* x = lval_qexpr();
    x = lval_add(x, lval_pop(a, 0));
    x = lval_join(x, lval_pop(a, 0));

    lval_del(a);
    return x;
}

lval* builtin_list(lenv* e, lval* a)
{
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_eval(lenv* e, lval* a)
{
    LASSERT_NUM("eval", a, 1);
    LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a)
{
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE("join", a, i, LVAL_QEXPR);
    }

    lval* x = lval_pop(a, 0);

    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* builtin_cmp(lenv* e, lval* a, char* op)
{
    LASSERT_NUM(op, a, 2);
    int r;
    if (strcmp(op, "==") == 0) {
        r = lval_eq(a->cell[0], a->cell[1]);
    }
    if (strcmp(op, "!=") == 0) {
        r = !lval_eq(a->cell[0], a->cell[1]);
    }
    if (strcmp(op, "&&") == 0) {
    }
    lval_del(a);
    return lval_bool(r);
}

lval* builtin_eq(lenv* e, lval* a)
{
    return builtin_cmp(e, a, "==");
}

lval* builtin_ne(lenv* e, lval* a)
{
    return builtin_cmp(e, a, "!=");
}

lval* builtin_and(lenv* e, lval* a)
{
    LASSERT_NUM("&&", a, 2);
    LASSERT_TYPE("&&", a, 0, LVAL_BOOL);
    LASSERT_TYPE("&&", a, 1, LVAL_BOOL);

    int r = a->cell[0]->bool && a->cell[1]->bool;

    lval_del(a);
    return lval_bool(r);
}

lval* builtin_or(lenv* e, lval* a)
{
    LASSERT_NUM("||", a, 2);
    LASSERT_TYPE("||", a, 0, LVAL_BOOL);
    LASSERT_TYPE("||", a, 1, LVAL_BOOL);

    int r = a->cell[0]->bool || a->cell[1]->bool;

    lval_del(a);
    return lval_bool(r);
}

lval* builtin_xor(lenv* e, lval* a)
{
    LASSERT_NUM("xor", a, 2);
    LASSERT_TYPE("xor", a, 0, LVAL_BOOL);
    LASSERT_TYPE("xor", a, 1, LVAL_BOOL);

    int r = a->cell[0]->bool ^ a->cell[1]->bool;
    lval_del(a);
    return lval_bool(r);
}

lval* builtin_not(lenv* e, lval* a)
{
    LASSERT_NUM("!", a, 1);
    LASSERT_TYPE("!", a, 0, LVAL_BOOL);

    int r = a->cell[0]->bool;
    lval_del(a);
    return lval_bool(!r);
}

lval* builtin_if(lenv* e, lval* a)
{
    LASSERT_NUM("if", a, 3);
    if (a->cell[0]->type != LVAL_BOOL && a->cell[0]->type != LVAL_NUM) {
        lval_del(a);
        return lval_err("Function 'if' cannot compare on %s. %s or %s expected",
                ltype_name(a->cell[0]->type),
                ltype_name(LVAL_BOOL),
                ltype_name(LVAL_NUM));
    }
    LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
    LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

    lval* x;
    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    if (a->cell[0]->type == LVAL_BOOL) {
        if (a->cell[0]->bool) {
            x = lval_eval(e, lval_pop(a, 1));
        } else {
            x = lval_eval(e, lval_pop(a, 2));
        }
    } else {
        if (a->cell[0]->num > 0) {
            x = lval_eval(e, lval_pop(a, 1));
        } else {
            x = lval_eval(e, lval_pop(a, 2));
        }
    }

    lval_del(a);
    return x;
}

lval* builtin_init(lenv* e, lval* a)
{
    /* Check error conditions */
    LASSERT_NUM("init", a, 1);
    LASSERT_TYPE("init", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("init", a, 0);

    lval* v = lval_take(a ,0);

    lval_del(lval_pop(v, v->count -1));

    return v;
}

lval* builtin_lambda(lenv* e, lval* a)
{
    LASSERT_NUM("\\", a, 2);
    LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                "Cannot define non-symbol. Got %s, expected %s.",
                ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
    }

    lval* formals = lval_pop(a, 0);
    lval* body = lval_pop(a, 0);
    lval_del(a);

    return lval_lambda(formals, body);
}

lval* builtin_var(lenv* e, lval* a, char* func)
{
    LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

    lval* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "Function '%s' cannot define non-symbol. Got %s, expected %s.",
                func, ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
    }

    LASSERT(a, (syms->count == a->count - 1),
            "Function '%s' passed to many arguments for symbols. Got %i, expected %i",
            func, syms->count, a->count - 1);

    for (int i = 0; i< syms->count; i++) {
        if (strcmp(func, "lvar") == 0) {
            lenv_def(e, syms->cell[i], a->cell[i+1]);
        }
        if (strcmp(func, "=") == 0) {
            lenv_put(e, syms->cell[i], a->cell[i+1]);
        }
    }

    lval_del(a);
    return lval_sexpr();
}

lval* builtin_def(lenv* e, lval* a)
{
    return builtin_var(e, a, "lvar");
}

lval* builtin_put(lenv* e, lval* a)
{
    return builtin_var(e, a, "=");
}

lval* builtin_env(lenv* e, lval* a)
{
    for (int i = 0; i < e->count; i++) {
        printf("%s\n", e->syms[i]);
    }
    return lval_pop(a, 0);
}

lval* builtin_inc(lenv* e, lval* a)
{
    LASSERT_NUM("++", a, 1);
    if (a->cell[0]->type != LVAL_NUM && a->cell[0]->type != LVAL_DEC) {
        lval_del(a);
        return lval_err("Cannot operate on %s. %s or %s expected",
                ltype_name(a->cell[0]->type),
                ltype_name(LVAL_NUM),
                ltype_name(LVAL_DEC));
    }

    lval* x = lval_pop(a, 0);

    if (x->type == LVAL_NUM) {
        int n = x->num + 1;
        lval_del(x);
        return lval_num(n);
    } else {
        double n = x->decimal + 1;
        lval_del(x);
        return lval_dec(n);
    }
}

lval* builtin_dec(lenv* e, lval* a)
{
    LASSERT_NUM("--", a, 1);
    if (a->cell[0]->type != LVAL_NUM && a->cell[0]->type != LVAL_DEC) {
        lval_del(a);
        return lval_err("Cannot operate on %s. %s or %s expected",
                ltype_name(a->cell[0]->type),
                ltype_name(LVAL_NUM),
                ltype_name(LVAL_DEC));
    }

    lval* x = lval_pop(a, 0);

    if (x->type == LVAL_NUM) {
        int n = x->num - 1;
        lval_del(x);
        return lval_num(n);
    } else {
        double n = x->decimal - 1;
        lval_del(x);
        return lval_dec(n);
    }

}

lval* builtin_add(lenv* e, lval* a)
{
    return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a)
{
    return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a)
{
    return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a)
{
    return builtin_op(e, a, "/");
}

lval* builtin_mod(lenv* e, lval* a)
{
    return builtin_op(e, a, "%");
}

lval* builtin_pow(lenv* e, lval* a)
{
    return builtin_op(e, a, "^");
}

lval* builtin_min(lenv* e, lval* a)
{
    return builtin_op(e, a, "min");
}

lval* builtin_max(lenv* e, lval* a)
{
    return builtin_op(e, a, "max");
}
lval* builtin_inc(lenv* e, lval* a);

lval* builtin_op(lenv* e, lval* a, char* op)
{
    /* Ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        if (a->cell[i]->type != LVAL_NUM && a->cell[i]->type != LVAL_DEC) {
            lval_del(a);
            return lval_err("Cannot operate on %s. %s or %s expected",
                    ltype_name(a->cell[i]->type),
                    ltype_name(LVAL_NUM),
                    ltype_name(LVAL_DEC));
        }
    }

    /* Pop the first element */
    lval* x = lval_pop(a, 0);

    if ((strcmp(op, "-") == 0) && (a->count == 0)) { 
        if (x->type == LVAL_NUM) x->num = -x->num;
        if (x->type == LVAL_DEC) x->decimal = -x->decimal;
    }

    while (a->count > 0) {
        lval* y = lval_pop(a, 0);

        if (x->type == LVAL_NUM && y->type == LVAL_NUM) {
            /* Operations */
            if (strcmp(op, "+") == 0) { x->num += y->num; }
            if (strcmp(op, "-") == 0) { x->num -= y->num; }
            if (strcmp(op, "*") == 0) { x->num *= y->num; }
            if (strcmp(op, "^") == 0) { x->num = pow(x->num, y->num); }
            if (strcmp(op, "min") == 0) { x->num = min(x->num, y->num); }
            if (strcmp(op, "max") == 0) { x->num = max(x->num, y->num); }
            if (strcmp(op, "%") == 0) {
                if (y->num == 0) {
                    lval_del(x); lval_del(y);
                    x = lval_err("Modulus by zero!");
                    break;
                }
                x->num = x->num % y->num;
            }
            if (strcmp(op, "/") == 0) {
                if (y->num == 0) {
                    lval_del(x); lval_del(y);
                    x = lval_err("Division by zero!");
                    break;
                }
                x->num /= y->num;
            }
        } else {
            /* One of the operands is a notnum */
            if (x->type == LVAL_DEC && y->type == LVAL_NUM) {
                y->type = LVAL_DEC;
                y->decimal = (double)(y->num);
            } else if (x->type == LVAL_NUM && y->type == LVAL_DEC) {
                x->type = LVAL_DEC;
                x->decimal = (double)(x->num);
            }
            /* Operations */
            if (strcmp(op, "+") == 0) { x->decimal += y->decimal; }
            if (strcmp(op, "-") == 0) { x->decimal -= y->decimal; }
            if (strcmp(op, "*") == 0) { x->decimal *= y->decimal; }
            if (strcmp(op, "^") == 0) { x->decimal = pow(x->decimal, y->decimal); }
            if (strcmp(op, "min") == 0) { x->decimal = fmin(x->decimal, y->decimal); }
            if (strcmp(op, "max") == 0) { x->decimal = fmax(x->decimal, y->decimal); }
            if (strcmp(op, "%") == 0) {
                if (y->decimal == 0) {
                    lval_del(x); lval_del(y);
                    x = lval_err("Modulus by zero!");
                    break;
                }
                x->decimal = fmod(x->decimal, y->decimal);
            }
            if (strcmp(op, "/") == 0) {
                if (y->decimal == 0) {
                    lval_del(x); lval_del(y);
                    x = lval_err("Division by zero!");
                    break;
                }
                x->decimal /= y->decimal;
            }
        }

        /* Delete y element */
        lval_del(y);
    }

    /* Delete input expression and return result */
    lval_del(a);
    return x;
}

lval* builtin_ln(lenv* e, lval* a)
{
    LASSERT_NUM("ln", a, 1);
    if (a->cell[0]->type != LVAL_NUM && a->cell[0]->type != LVAL_DEC) {
        lval_del(a);
        return lval_err("Cannot operate on non-number. %s or %s expected",
                ltype_name(LVAL_NUM), ltype_name(LVAL_DEC));
    }

    double r;
    if (a->cell[0]->type == LVAL_NUM) {
        r = log(a->cell[0]->num);
    } else {
        r = log(a->cell[0]->decimal);
    }

    lval_del(a);
    return lval_dec(r);
}

lval* builtin_ceil(lenv* e, lval* a)
{
    LASSERT_NUM("ln", a, 1);
    if (a->cell[0]->type != LVAL_NUM && a->cell[0]->type != LVAL_DEC) {
        lval_del(a);
        return lval_err("Cannot operate on non-number. %s or %s expected",
                ltype_name(LVAL_NUM), ltype_name(LVAL_DEC));
    }

    int r;
    if (a->cell[0]->type == LVAL_NUM) {
        r = a->cell[0]->num;
    } else {
        r = ceil(a->cell[0]->decimal);
    }
    lval_del(a);
    return lval_num(r);
}

lval* builtin_floor(lenv* e, lval* a)
{
    LASSERT_NUM("ln", a, 1);
    if (a->cell[0]->type != LVAL_NUM && a->cell[0]->type != LVAL_DEC) {
        lval_del(a);
        return lval_err("Cannot operate on non-number. %s or %s expected",
                ltype_name(LVAL_NUM), ltype_name(LVAL_DEC));
    }

    int r;
    if (a->cell[0]->type == LVAL_NUM) {
        r = a->cell[0]->num;
    } else {
        r = floor(a->cell[0]->decimal);
    }
    lval_del(a);
    return lval_num(r);
}

lval* builtin_rand(lenv* e, lval* a)
{
    LASSERT_NUM("rand", a, 1);
    LASSERT_TYPE("rand", a, 0, LVAL_NUM);

    double r;
    srand(time(NULL));

    if (a->cell[0]->num < 1) {
        r = 1;
    } else {
        r = rand() % a->cell[0]->num + (float)(rand() % 1000000) / 1000000;
    }

    lval_del(a);
    return lval_dec(r);
}

lval* builtin_leftshift(lenv* e, lval* a)
{
    LASSERT_NUM("<<", a, 2);
    LASSERT_TYPE("<<", a, 0, LVAL_NUM);
    LASSERT_TYPE("<<", a, 1, LVAL_NUM);

    int r;
    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    r = x->num << y->num;
    lval_del(x);
    lval_del(y);
    return lval_num(r);
}

lval* builtin_rightshift(lenv* e, lval* a)
{
    LASSERT_NUM(">>", a, 2);
    LASSERT_TYPE(">>", a, 0, LVAL_NUM);
    LASSERT_TYPE(">>", a, 1, LVAL_NUM);

    int r;
    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    r = x->num >> y->num;
    lval_del(x);
    lval_del(y);
    return lval_num(r);
}

lval* builtin_bitwiseand(lenv* e, lval* a)
{
    LASSERT_NUM("&", a, 2);
    LASSERT_TYPE("&", a, 0, LVAL_NUM);
    LASSERT_TYPE("&", a, 1, LVAL_NUM);

    int r;
    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    r = x->num & y->num;
    lval_del(x);
    lval_del(y);
    return lval_num(r);
}

lval* builtin_bitwiseor(lenv* e, lval* a)
{
    LASSERT_NUM("|", a, 2);
    LASSERT_TYPE("|", a, 0, LVAL_NUM);
    LASSERT_TYPE("|", a, 1, LVAL_NUM);

    int r;
    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    r = x->num | y->num;
    lval_del(x);
    lval_del(y);
    return lval_num(r);
}

lval* builtin_bitwisexor(lenv* e, lval* a)
{
    LASSERT_NUM("^", a, 2);
    LASSERT_TYPE("^", a, 0, LVAL_NUM);
    LASSERT_TYPE("^", a, 1, LVAL_NUM);

    int r;
    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    r = x->num ^ y->num;
    lval_del(x);
    lval_del(y);
    return lval_num(r);
}

lval* builtin_gt(lenv* e, lval* a)
{
    return builtin_ord(e, a, ">");
}

lval* builtin_lt(lenv* e, lval* a)
{
    return builtin_ord(e, a, "<");
}

lval* builtin_ge(lenv* e, lval* a)
{
    return builtin_ord(e, a, "<=");
}

lval* builtin_le(lenv* e, lval* a)
{
    return builtin_ord(e, a, ">=");
}

lval* builtin_ord(lenv* e, lval* a, char* op)
{
    LASSERT_NUM(op, a, 2);
    /* Ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        if (a->cell[i]->type != LVAL_NUM && a->cell[i]->type != LVAL_DEC) {
            lval_del(a);
            return lval_err("Cannot operate on non-number. %s or %s expected",
                    ltype_name(LVAL_NUM), ltype_name(LVAL_DEC));
        }
    }

    int r;
    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    if (x->type == LVAL_NUM && y->type == LVAL_NUM) {
        if (strcmp(op, ">") == 0) {
            r = (x->num > y->num);
        }
        if (strcmp(op, "<") == 0) {
            r = (x->num < y->num);
        }
        if (strcmp(op, ">=") == 0) {
            r = (x->num >= y->num);
        }
        if (strcmp(op, "<=") == 0) {
            r = (x->num <= y->num);
        }
    } else {
        /* One of the operands is a notnum */
        if (x->type == LVAL_DEC && y->type == LVAL_NUM) {
            y->type = LVAL_DEC;
            y->decimal = (double)(y->num);
        } else if (x->type == LVAL_NUM && y->type == LVAL_DEC) {
            x->type = LVAL_DEC;
            x->decimal = (double)(x->num);
        }
        if (strcmp(op, ">") == 0) {
            r = (x->decimal > y->decimal);
        }
        if (strcmp(op, "<") == 0) {
            r = (x->decimal < y->decimal);
        }
        if (strcmp(op, ">=") == 0) {
            r = (x->decimal >= y->decimal);
        }
        if (strcmp(op, "<=") == 0) {
            r = (x->decimal <= y->decimal);
        }
    }

    lval_del(a);
    return lval_bool(r);
}

lval* builtin_load(lenv* e, lval* a)
{
    LASSERT_NUM("load", a, 1);
    LASSERT_TYPE("load", a, 0, LVAL_STR);

    mpc_result_t r;
    if (!strstr(a->cell[0]->str, ".zl")) {
        strcat(a->cell[0]->str, ".zl");
    }
    if (mpc_parse_contents(a->cell[0]->str, Zlango, &r)) {
        /* Read contents */
        lval* expr = lval_read(r.output);
        mpc_ast_delete(r.output);

        while (expr->count) {
            lval* x = lval_eval(e, lval_pop(expr, 0));
            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }

        lval_del(expr);
        lval_del(a);

        return lval_sexpr();
    } else {
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        lval* err = lval_err("Could not load Library %s", err_msg);
        free(err_msg);
        lval_del(a);

        return err;
    }
}

lval* builtin_print(lenv* e, lval* a)
{
    for (int i = 0; i < a->count; i++) {
        lval_print(a->cell[i]);
        putchar(' ');
    }
    putchar('\n');
    lval_del(a);

    return lval_sexpr();
}

lval* builtin_exit(lenv* e, lval* a)
{
    lenv_del(e);
    lval_del(a);
    exit(0);
}

lval* builtin_error(lenv* e, lval* a)
{
    LASSERT_NUM("error", a, 1);
    LASSERT_TYPE("error", a, 0, LVAL_STR);

    lval* err = lval_err(a->cell[0]->str);

    lval_del(a);

    return err;
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func)
{
    lval* k = lval_sym(name);
    lval* v = lval_fun(func);
    v->is_builtin = 1;
    lenv_put(e, k ,v);
    lval_del(k);
    lval_del(v);
}

void lenv_add_builtin_var(lenv* e, char* name, lval* val)
{
    lval* k = lval_sym(name);
    lval* v = lval_copy(val);
    v->is_builtin = 1;
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}

void lenv_add_builtins(lenv* e)
{
    /* Builtin vars */
    lenv_add_builtin_var(e, "true", lval_bool(1));
    lenv_add_builtin_var(e, "false", lval_bool(0));
    lenv_add_builtin_var(e, "nil", lval_qexpr());

    /* String Functions */
    lenv_add_builtin(e, "load", builtin_load);
    lenv_add_builtin(e, "error", builtin_error);
    lenv_add_builtin(e, "put", builtin_print);
    lenv_add_builtin(e, "eval", builtin_eval);

    /* Assignments */
    lenv_add_builtin(e, "\\", builtin_lambda);
    lenv_add_builtin(e, "lvar", builtin_def);
    lenv_add_builtin(e, "=", builtin_put);
    lenv_add_builtin(e, "env", builtin_env);

    /* List operations */
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "init", builtin_init);

    /* Arithmetic */
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "%", builtin_mod);
    lenv_add_builtin(e, "++", builtin_inc);
    lenv_add_builtin(e, "--", builtin_dec);
    lenv_add_builtin(e, "pow", builtin_pow);
    lenv_add_builtin(e, "ln", builtin_ln);
    lenv_add_builtin(e, "ceil", builtin_ceil);
    lenv_add_builtin(e, "floor", builtin_floor);
    lenv_add_builtin(e, "min", builtin_min);
    lenv_add_builtin(e, "max", builtin_max);
    lenv_add_builtin(e, "rand", builtin_rand);

    /* Bit shift */
    lenv_add_builtin(e, "<<", builtin_leftshift);
    lenv_add_builtin(e, ">>", builtin_rightshift);
    lenv_add_builtin(e, "&", builtin_bitwiseand);
    lenv_add_builtin(e, "|", builtin_bitwiseor);
    lenv_add_builtin(e, "^", builtin_bitwisexor);

    /* Conditionals */
    lenv_add_builtin(e, "if", builtin_if);
    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_ne);
    lenv_add_builtin(e, ">", builtin_gt);
    lenv_add_builtin(e, "<", builtin_lt);
    lenv_add_builtin(e, ">=", builtin_ge);
    lenv_add_builtin(e, "<=", builtin_le);
    lenv_add_builtin(e, "&&", builtin_and);
    lenv_add_builtin(e, "||", builtin_or);
    lenv_add_builtin(e, "!", builtin_not);
    lenv_add_builtin(e, "xor", builtin_xor);

    /* Other functions */
    lenv_add_builtin(e, "exit", builtin_exit);
}
