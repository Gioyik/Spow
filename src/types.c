#include "../include/types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>

#include "../include/assert.h"
#include "../include/builtins.h"
#include "../include/print.h"
#include "../include/util.h"

char* zlval_type_name(zlval_type_t t) {
    switch (t) {
        case ZLVAL_ERR: return "Error";
        case ZLVAL_INT: return "Integer";
        case ZLVAL_FLOAT: return "Float";
        case ZLVAL_BUILTIN: return "Builtin";
        case ZLVAL_FN: return "Function";
        case ZLVAL_MACRO: return "Macro";
        case ZLVAL_SYM: return "Symbol";
        case ZLVAL_QSYM: return "Q-Symbol";
        case ZLVAL_STR: return "String";
        case ZLVAL_BOOL: return "Boolean";
        case ZLVAL_DICT: return "Dictionary";
        case ZLVAL_SEXPR: return "S-Expression";
        case ZLVAL_QEXPR: return "Q-Expression";
        case ZLVAL_EEXPR: return "E-Expression";
        case ZLVAL_CEXPR: return "C-Expression";
        default: return "Unknown";
    }
}

char* zlval_type_sysname(zlval_type_t t) {
    switch (t) {
        case ZLVAL_ERR: return "err";
        case ZLVAL_INT: return "int";
        case ZLVAL_FLOAT: return "float";
        case ZLVAL_BUILTIN: return "builtin";
        case ZLVAL_FN: return "fn";
        case ZLVAL_MACRO: return "macro";
        case ZLVAL_SYM: return "sym";
        case ZLVAL_QSYM: return "qsym";
        case ZLVAL_STR: return "str";
        case ZLVAL_BOOL: return "bool";
        case ZLVAL_DICT: return "dict";
        case ZLVAL_SEXPR: return "sexpr";
        case ZLVAL_QEXPR: return "qexpr";
        case ZLVAL_EEXPR: return "eexpr";
        case ZLVAL_CEXPR: return "cexpr";
        default: return "unknown";
    }
}

zlval_type_t zlval_parse_sysname(const char* sysname) {
    /* Ordering reflects most common types first */
    if (streq(sysname, "int")) {
        return ZLVAL_INT;
    } else if (streq(sysname, "float")) {
        return ZLVAL_FLOAT;
    } else if (streq(sysname, "str")) {
        return ZLVAL_STR;
    } else if (streq(sysname, "bool")) {
        return ZLVAL_BOOL;
    } else if (streq(sysname, "qsym")) {
        return ZLVAL_QSYM;
    } else if (streq(sysname, "qexpr")) {
        return ZLVAL_QEXPR;
    } else if (streq(sysname, "err")) {
        return ZLVAL_ERR;
    } else if (streq(sysname, "builtin")) {
        return ZLVAL_BUILTIN;
    } else if (streq(sysname, "fn")) {
        return ZLVAL_FN;
    } else if (streq(sysname, "macro")) {
        return ZLVAL_MACRO;
    } else if (streq(sysname, "sym")) {
        return ZLVAL_SYM;
    } else if (streq(sysname, "dict")) {
        return ZLVAL_DICT;
    } else if (streq(sysname, "sexpr")) {
        return ZLVAL_SEXPR;
    } else if (streq(sysname, "eexpr")) {
        return ZLVAL_EEXPR;
    } else if (streq(sysname, "cexpr")) {
        return ZLVAL_CEXPR;
    } else {
        errno = EINVAL;
        return 0;
    }
}

zlval* zlval_err(const char* fmt, ...) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->err = safe_malloc(512);
    vsnprintf(v->err, 512, fmt, va);

    unsigned int l = strlen(v->err) + 1;
    v->err = realloc(v->err, l);
    v->err[l - 1] = '\0';

    va_end(va);

    return v;
}

zlval* zlval_int(long x) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_INT;
    v->lng = x;
    return v;
}

zlval* zlval_float(double x) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_FLOAT;
    v->dbl = x;
    return v;
}

static zlval* zlval_sym_base(const char* s) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->length = strlen(s);
    v->sym = safe_malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

zlval* zlval_sym(const char* s) {
    zlval* v = zlval_sym_base(s);
    v->type = ZLVAL_SYM;
    return v;
}

zlval* zlval_qsym(const char* s) {
    zlval* v = zlval_sym_base(s);
    v->type = ZLVAL_QSYM;
    return v;
}

zlval* zlval_str(const char* s) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_STR;
    v->length = strlen(s);
    v->str = safe_malloc(v->length + 1);
    strcpy(v->str, s);
    return v;
}

zlval* zlval_bool(bool b) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_BOOL;
    v->bln = b;
    return v;
}

zlval* zlval_fun(const zlbuiltin builtin, const char* builtin_name) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_BUILTIN;
    v->builtin = builtin;
    v->builtin_name = safe_malloc(strlen(builtin_name) + 1);
    strcpy(v->builtin_name, builtin_name);
    return v;
}

zlval* zlval_lambda(zlenv* closure, zlval* formals, zlval* body) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_FN;
    v->env = zlenv_new();
    v->env->parent = closure;
    v->env->parent->references++;
    v->formals = formals;
    v->body = body;
    v->called = false;
    return v;
}

zlval* zlval_macro(zlenv* closure, zlval* formals, zlval* body) {
    zlval* v = zlval_lambda(closure, formals, body);
    v->type = ZLVAL_MACRO;
    return v;
}

static void* zlval_copy_proxy(const void* v) {
    return zlval_copy(v);
}

static void zlval_del_proxy(void* v) {
    zlval_del(v);
}

zlval* zlval_dict(void) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_DICT;
    v->count = 0;
    v->length = 0;
    v->d = dict_new(zlval_copy_proxy, zlval_del_proxy);
    return v;
}

zlval* zlval_sexpr(void) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_SEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

zlval* zlval_qexpr(void) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_QEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

zlval* zlval_eexpr(void) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_EEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

zlval* zlval_cexpr(void) {
    zlval* v = safe_malloc(sizeof(zlval));
    v->type = ZLVAL_CEXPR;
    v->count = 0;
    v->length = 0;
    v->cell = NULL;
    return v;
}

void zlval_del(zlval* v) {
    switch (v->type) {
        case ZLVAL_INT:
            break;

        case ZLVAL_FLOAT:
            break;

        case ZLVAL_BUILTIN:
            free(v->builtin_name);
            break;

        case ZLVAL_FN:
        case ZLVAL_MACRO:
            zlenv_del(v->env);
            zlval_del(v->formals);
            zlval_del(v->body);
            break;

        case ZLVAL_ERR:
            free(v->err);
            break;

        case ZLVAL_SYM:
        case ZLVAL_QSYM:
            free(v->sym);
            break;

        case ZLVAL_STR:
            free(v->str);
            break;

        case ZLVAL_BOOL:
            break;

        case ZLVAL_DICT:
            dict_del(v->d);
            break;

        case ZLVAL_EEXPR:
        case ZLVAL_SEXPR:
        case ZLVAL_QEXPR:
        case ZLVAL_CEXPR:
            for (int i = 0; i < v->count; i++) {
                zlval_del(v->cell[i]);
            }
            free(v->cell);
            break;
    }

    free(v);
}

zlval* zlval_add(zlval* v, zlval* x) {
    v->count++;
    v->length++;
    v->cell = realloc(v->cell, sizeof(zlval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

zlval* zlval_add_front(zlval* v, zlval* x) {
    v->count++;
    v->length++;
    v->cell = realloc(v->cell, sizeof(zlval*) * v->count);
    if (v->count > 1) {
        memmove(&v->cell[1], &v->cell[0], sizeof(zlval*) * (v->count - 1));
    }
    v->cell[0] = x;
    return v;
}

zlval* zlval_add_dict(zlval* x, zlval* k, zlval* v) {
    dict_put(x->d, k->sym, v);
    x->count = x->length = dict_count(x->d);
    return x;
}

zlval* zlval_get_dict(zlval* x, zlval* k) {
    return dict_get(x->d, k->sym);
}

zlval* zlval_rm_dict(zlval* x, zlval* k) {
    dict_rm(x->d, k->sym);
    x->count = x->length = dict_count(x->d);
    return x;
}

bool zlval_haskey_dict(zlval* x, zlval* k) {
    return dict_index(x->d, k->sym) != -1;
}

zlval* zlval_keys_dict(zlval* x) {
    int count = dict_count(x->d);
    char** keys = dict_all_keys(x->d);

    zlval* v = zlval_qexpr();
    for (int i = 0; i < count; i++) {
        zlval_add(v, zlval_qsym(keys[i]));
    }

    free(keys);
    return v;
}

zlval* zlval_vals_dict(zlval* x) {
    int count = dict_count(x->d);
    zlval** vals = (zlval**)dict_all_vals(x->d);

    zlval* v = zlval_qexpr();
    for (int i = 0; i < count; i++) {
        zlval_add(v, zlval_copy(vals[i]));
    }

    free(vals);
    return v;
}

zlval* zlval_pop(zlval* v, int i) {
    zlval* x = v->cell[i];

    memmove(&v->cell[i], &v->cell[i + 1], sizeof(zlval*) * (v->count - i - 1));
    v->count--;
    v->length--;

    v->cell = realloc(v->cell, sizeof(zlval*) * v->count);
    return x;
}

zlval* zlval_take(zlval* v, int i) {
    zlval* x = zlval_pop(v, i);
    zlval_del(v);
    return x;
}

zlval* zlval_join(zlval* x, zlval* y) {
    while (y->count) {
        x = zlval_add(x, zlval_pop(y, 0));
    }

    zlval_del(y);
    return x;
}

zlval* zlval_insert(zlval* x, zlval* y, int i) {
    x->count++;
    x->length++;
    x->cell = realloc(x->cell, sizeof(zlval*) * x->count);

    memmove(&x->cell[i + 1], &x->cell[i], sizeof(zlval*) * (x->count - i - 1));
    x->cell[i] = y;
    return x;
}

zlval* zlval_shift(zlval* x, zlval* y, int i) {
    while (y->count) {
        x = zlval_insert(x, zlval_pop(y, y->count - 1), i);
    }

    zlval_del(y);
    return x;
}

static zlval* zlval_reverse_qexpr(zlval* x) {
    zlval* y = zlval_qexpr();
    while (x->count) {
        y = zlval_add(y, zlval_pop(x, x->count - 1));
    }
    zlval_del(x);
    return y;
}

static zlval* zlval_reverse_qsym(zlval* x) {
    char* reversed = strrev(x->sym);
    free(x->sym);
    x->sym = reversed;
    return x;
}

static zlval* zlval_reverse_str(zlval* x) {
    char* reversed = strrev(x->str);
    free(x->str);
    x->str = reversed;
    return x;
}

zlval* zlval_reverse(zlval* x) {
    if (x->type == ZLVAL_QEXPR) {
        return zlval_reverse_qexpr(x);
    } else if (x->type == ZLVAL_STR) {
        return zlval_reverse_str(x);
    } else {
        return zlval_reverse_qsym(x);
    }
}

static zlval* zlval_slice_step_qexpr(zlval* x, int start, int end, int step) {
    /* Remove from front */
    while (start) {
        zlval_del(zlval_pop(x, 0));
        start--; end--;
    }
    /* Remove from back */
    while (end < x->count) {
        zlval_del(zlval_pop(x, end));
    }
    /* Remove by steps */
    if (step > 1 && x->count) {
        int final_count = x->count / step + (x->count % step == 0 ? 0 : 1),
            multiplier = 0,
            removal_offset = 0;

        while (x->count > final_count) {
            for (int i = 1; i < step; i++) {
                int idx = multiplier * step + i - removal_offset;
                zlval_del(zlval_pop(x, idx));
                removal_offset++;
            }
            multiplier++;
        }
    }
    return x;
}

static zlval* zlval_slice_step_str(zlval* x, int start, int end, int step) {
    char* sliced = strsubstr(x->str, start, end);
    if (step > 1 && strlen(sliced)) {
        char* stepped = strstep(sliced, step);
        free(sliced);
        sliced = stepped;
    }
    free(x->str);
    x->str = sliced;
    return x;
}

static zlval* zlval_slice_step_qsym(zlval* x, int start, int end, int step) {
    char* sliced = strsubstr(x->sym, start, end);
    if (step > 1 && strlen(sliced)) {
        char* stepped = strstep(sliced, step);
        free(sliced);
        sliced = stepped;
    }
    free(x->sym);
    x->sym = sliced;
    return x;
}

zlval* zlval_slice_step(zlval* x, int start, int end, int step) {
    if (x->type == ZLVAL_QEXPR) {
        return zlval_slice_step_qexpr(x, start, end, step);
    } else if (x->type == ZLVAL_STR) {
        return zlval_slice_step_str(x, start, end, step);
    } else {
        return zlval_slice_step_qsym(x, start, end, step);
    }
}

zlval* zlval_slice(zlval* x, int start, int end) {
    return zlval_slice_step(x, start, end, 1);
}

void zlval_maybe_promote_numeric(zlval* a, zlval* b) {
    if (!(ISNUMERIC(a->type) && ISNUMERIC(b->type))) {
        return;
    }
    if (a->type == ZLVAL_FLOAT || b->type == ZLVAL_FLOAT) {
        zlval_promote_numeric(a);
        zlval_promote_numeric(b);
    }
}

void zlval_promote_numeric(zlval* x) {
    if (x->type != ZLVAL_FLOAT) {
        x->type = ZLVAL_FLOAT;
        x->dbl = (double)x->lng;
    }
}

void zlval_demote_numeric(zlval* x) {
    if (x->type != ZLVAL_INT) {
        x->type = ZLVAL_INT;
        x->lng = (long)x->dbl;
    }
}

zlval* zlval_copy(const zlval* v) {
    zlval* x = safe_malloc(sizeof(zlval));
    x->type = v->type;

    switch (v->type) {
        case ZLVAL_BUILTIN:
            x->builtin = v->builtin;
            x->builtin_name = safe_malloc(strlen(v->builtin_name) + 1);
            strcpy(x->builtin_name, v->builtin_name);
            break;

        case ZLVAL_FN:
        case ZLVAL_MACRO:
            x->env = zlenv_copy(v->env);
            x->formals = zlval_copy(v->formals);
            x->body = zlval_copy(v->body);
            x->called = v->called;
            break;

        case ZLVAL_INT:
            x->lng = v->lng;
            break;

        case ZLVAL_FLOAT:
            x->dbl = v->dbl;
            break;

        case ZLVAL_ERR:
            x->err = safe_malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;

        case ZLVAL_SYM:
        case ZLVAL_QSYM:
            x->length = v->length;
            x->sym = safe_malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;

        case ZLVAL_STR:
            x->length = v->length;
            x->str = safe_malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;

        case ZLVAL_BOOL:
            x->bln = v->bln;
            break;

        case ZLVAL_DICT:
            x->count = v->count;
            x->length = v->length;
            x->d = dict_copy(v->d);
            break;

        case ZLVAL_SEXPR:
        case ZLVAL_QEXPR:
        case ZLVAL_EEXPR:
        case ZLVAL_CEXPR:
            x->count = v->count;
            x->length = v->length;
            x->cell = safe_malloc(sizeof(zlval*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = zlval_copy(v->cell[i]);
            }
            break;
    }

    return x;
}

zlval* zlval_convert(zlval_type_t t, const zlval* v) {
    if (v->type == t) {
        return zlval_copy(v);
    }

    switch (t) {
        case ZLVAL_INT:
            switch (v->type) {
                case ZLVAL_FLOAT:
                    return zlval_int((long)v->dbl);
                    break;

                case ZLVAL_STR:
                    {
                        errno = 0;
                        char* strend = v->str;
                        long x = strtol(v->str, &strend, 10);
                        return errno != ERANGE && *strend == '\0' ? zlval_int(x) : zlval_err("invalid number: %s", v->str);
                    }
                    break;

                case ZLVAL_BOOL:
                    return zlval_int(v->bln);
                    break;

                default:
                    return zlval_err("a direct conversion from type %s to type %s does not exist",
                            zlval_type_name(v->type), zlval_type_name(t));
                    break;
            }
            break;

        case ZLVAL_FLOAT:
            switch (v->type) {
                case ZLVAL_INT:
                    return zlval_float((double)v->lng);
                    break;

                case ZLVAL_STR:
                    {
                        errno = 0;
                        char* strend = v->str;
                        double x = strtod(v->str, &strend);
                        return errno != ERANGE && *strend == '\0' ? zlval_float(x) : zlval_err("invalid float: %s", v->str);
                    }
                    break;

                case ZLVAL_BOOL:
                    return zlval_float((double)v->bln);
                    break;

                default:
                    return zlval_err("a direct conversion from type %s to type %s does not exist",
                            zlval_type_name(v->type), zlval_type_name(t));
                    break;
            }
            break;

        case ZLVAL_STR:
            if (v->type == ZLVAL_QSYM) {
                return zlval_str(v->sym);
            } else {
                char* str = zlval_to_str(v);
                zlval* res = zlval_str(str);
                free(str);
                return res;
            }
            break;

        case ZLVAL_BOOL:
            switch (v->type) {
                case ZLVAL_INT:
                    return zlval_bool(v->lng != 0 ? true : false);
                    break;

                case ZLVAL_FLOAT:
                    return zlval_bool(v->dbl != 0.0 ? true : false);
                    break;

                default:
                    return zlval_err("a direct conversion from type %s to type %s does not exist",
                            zlval_type_name(v->type), zlval_type_name(t));
                    break;
            }
            break;

        case ZLVAL_QSYM:
            switch (v->type) {
                case ZLVAL_STR:
                    return zlval_qsym(v->str);
                    break;

                default:
                    return zlval_err("a direct conversion from type %s to type %s does not exist",
                            zlval_type_name(v->type), zlval_type_name(t));
                    break;
            }
            break;

        default:
            return zlval_err("no type can be directly converted to type %s", zlval_type_name(t));
            break;
    }
}

bool zlval_eq(zlval* x, zlval* y) {
    zlval_maybe_promote_numeric(x, y);
    if (x->type != y->type) {
        return false;
    }

    switch (x->type) {
        case ZLVAL_BUILTIN:
            return y->type == ZLVAL_BUILTIN && x->builtin == y->builtin;
            break;

        case ZLVAL_FN:
        case ZLVAL_MACRO:
            return y->type == x->type && zlval_eq(x->formals, y->formals) && zlval_eq(x->body, y->body);
            break;

        case ZLVAL_INT:
            return x->lng == y->lng;
            break;

        case ZLVAL_FLOAT:
            return x->dbl == y->dbl;
            break;

        case ZLVAL_ERR:
            return streq(x->err, y->err);
            break;

        case ZLVAL_SYM:
        case ZLVAL_QSYM:
            return x->length == y->length && streq(x->sym, y->sym);
            break;

        case ZLVAL_STR:
            return x->length == y->length && streq(x->str, y->str);
            break;

        case ZLVAL_BOOL:
            return x->bln == y->bln;
            break;

        case ZLVAL_DICT:
            return dict_equal(x->d, y->d);
            break;

        case ZLVAL_SEXPR:
        case ZLVAL_QEXPR:
        case ZLVAL_EEXPR:
        case ZLVAL_CEXPR:
            if (x->count != y->count) {
                return false;
            }
            for (int i = 0; i < x->count; i++) {
                if (!zlval_eq(x->cell[i], y->cell[i])) {
                    return false;
                }
            }
            return true;
            break;
    }
    return false;
}

bool is_zlval_empty_qexpr(zlval* x) {
    return x->type == ZLVAL_QEXPR && x->count == 0;
}

zlenv* zlenv_new(void) {
    zlenv* e = safe_malloc(sizeof(zlenv));
    e->parent = NULL;
    e->internal_dict = dict_new(zlval_copy_proxy, zlval_del_proxy);
    e->top_level = false;
    e->references = 1;
    return e;
}

zlenv* zlenv_new_top_level(void) {
    zlenv* e = zlenv_new();
    e->top_level = true;
    zlenv_add_builtins(e);
    return e;
}

void zlenv_del(zlenv* e) {
    e->references--;

    if (e->references <= 0 && !e->top_level) {
        if (e->parent && e->parent->references >= 1) {
            zlenv_del(e->parent);
        }

        dict_del(e->internal_dict);
        free(e);
    }
}

void zlenv_del_top_level(zlenv* e) {
    e->references = 1;
    e->top_level = false;
    zlenv_del(e);
}

int zlenv_index(zlenv* e, zlval* k) {
    return dict_index(e->internal_dict, k->sym);
}

static zlval* zlenv_lookup(zlenv* e, char* k) {
    int i = dict_index(e->internal_dict, k);
    if (i != -1) {
        return dict_get_at(e->internal_dict, i);
    }

    /* check parent if not found */
    if (e->parent) {
        return zlenv_lookup(e->parent, k);
    } else {
        return zlval_err("unbound symbol '%s'", k);
    }
}

zlval* zlenv_get(zlenv* e, zlval* k) {
    return zlenv_lookup(e, k->sym);
}

void zlenv_put(zlenv* e, zlval* k, zlval* v) {
    dict_put(e->internal_dict, k->sym, v);
}

void zlenv_put_global(zlenv* e, zlval* k, zlval* v) {
    while (e->parent) {
        e = e->parent;
    }
    zlenv_put(e, k, v);
}

zlenv* zlenv_copy(zlenv* e) {
    zlenv* n = safe_malloc(sizeof(zlenv));
    n->parent = e->parent;
    if (n->parent) {
        n->parent->references++;
    }
    n->internal_dict = dict_copy(e->internal_dict);
    n->top_level = e->top_level;
    n->references = 1;

    return n;
}

void zlenv_add_builtin(zlenv* e, char* name, zlbuiltin builtin) {
    zlval* k = zlval_sym(name);
    zlval* v = zlval_fun(builtin, name);
    zlenv_put(e, k, v);
    zlval_del(k);
    zlval_del(v);
}

void zlenv_add_builtins(zlenv* e) {
    zlenv_add_builtin(e, "+", builtin_add);
    zlenv_add_builtin(e, "-", builtin_sub);
    zlenv_add_builtin(e, "*", builtin_mul);
    zlenv_add_builtin(e, "/", builtin_div);
    zlenv_add_builtin(e, "//", builtin_trunc_div);
    zlenv_add_builtin(e, "%", builtin_mod);
    zlenv_add_builtin(e, "^", builtin_pow);

    zlenv_add_builtin(e, ">", builtin_gt);
    zlenv_add_builtin(e, ">=", builtin_gte);
    zlenv_add_builtin(e, "<", builtin_lt);
    zlenv_add_builtin(e, "<=", builtin_lte);

    zlenv_add_builtin(e, "==", builtin_eq);
    zlenv_add_builtin(e, "!=", builtin_neq);

    zlenv_add_builtin(e, "and", builtin_and);
    zlenv_add_builtin(e, "or", builtin_or);
    zlenv_add_builtin(e, "not", builtin_not);

    zlenv_add_builtin(e, "head", builtin_head);
    zlenv_add_builtin(e, "qhead", builtin_qhead);
    zlenv_add_builtin(e, "tail", builtin_tail);
    zlenv_add_builtin(e, "first", builtin_first);
    zlenv_add_builtin(e, "last", builtin_last);
    zlenv_add_builtin(e, "list", builtin_list);
    zlenv_add_builtin(e, "eval", builtin_eval);
    zlenv_add_builtin(e, "append", builtin_append);
    zlenv_add_builtin(e, "cons", builtin_cons);
    zlenv_add_builtin(e, "except-last", builtin_exceptlast);
    zlenv_add_builtin(e, "dict-get", builtin_dictget);
    zlenv_add_builtin(e, "dict-set", builtin_dictset);
    zlenv_add_builtin(e, "dict-del", builtin_dictdel);
    zlenv_add_builtin(e, "dict-haskey?", builtin_dicthaskey);
    zlenv_add_builtin(e, "dict-keys", builtin_dictkeys);
    zlenv_add_builtin(e, "dict-vals", builtin_dictvals);

    zlenv_add_builtin(e, "len", builtin_len);
    zlenv_add_builtin(e, "reverse", builtin_reverse);
    zlenv_add_builtin(e, "slice", builtin_slice);

    zlenv_add_builtin(e, "if", builtin_if);
    zlenv_add_builtin(e, "define", builtin_define);
    zlenv_add_builtin(e, "global", builtin_global);

    zlenv_add_builtin(e, "let", builtin_let);
    zlenv_add_builtin(e, "fn", builtin_lambda);
    zlenv_add_builtin(e, "macro", builtin_macro);

    zlenv_add_builtin(e, "typeof", builtin_typeof);
    zlenv_add_builtin(e, "convert", builtin_convert);
    zlenv_add_builtin(e, "import", builtin_import);
    zlenv_add_builtin(e, "print", builtin_print);
    zlenv_add_builtin(e, "println", builtin_println);
    zlenv_add_builtin(e, "random", builtin_random);
    zlenv_add_builtin(e, "error", builtin_error);
    zlenv_add_builtin(e, "exit", builtin_exit);
}