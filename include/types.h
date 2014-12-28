#ifndef ZL_TYPES_H
#define ZL_TYPES_H

#include <stdbool.h>

#include "dict.h"

struct zlval;
struct zlenv;
typedef struct zlval zlval;
typedef struct zlenv zlenv;

/* zlval types */
typedef enum {
    /* The order of numeric types is important */
    ZLVAL_INT,
    ZLVAL_FLOAT,

    ZLVAL_ERR,
    ZLVAL_SYM,
    ZLVAL_QSYM,
    ZLVAL_STR,
    ZLVAL_BOOL,
    ZLVAL_BUILTIN,
    ZLVAL_FN,
    ZLVAL_MACRO,
    ZLVAL_DICT,

    ZLVAL_SEXPR,
    ZLVAL_QEXPR,
    ZLVAL_EEXPR,
    ZLVAL_CEXPR
} zlval_type_t;

#define ISNUMERIC(t) (t == ZLVAL_INT || t == ZLVAL_FLOAT)
#define ISORDEREDCOLLECTION(t) (t == ZLVAL_QEXPR || t == ZLVAL_STR || t == ZLVAL_QSYM)
#define ISCOLLECTION(t) (t == ZLVAL_QEXPR || t == ZLVAL_STR || t == ZLVAL_QSYM || t == ZLVAL_DICT)
#define ISEXPR(t) (t == ZLVAL_QEXPR || t == ZLVAL_SEXPR)
#define ISCALLABLE(t) (t == ZLVAL_BUILTIN || t == ZLVAL_FN || t == ZLVAL_MACRO)

char* zlval_type_name(zlval_type_t t);
char* zlval_type_sysname(zlval_type_t t);
zlval_type_t zlval_parse_sysname(const char* sysname);

/* function pointer */
typedef zlval*(*zlbuiltin)(zlenv*, zlval*);

struct zlval {
    zlval_type_t type;
    int count;
    zlval** cell;

    /* collection types have length */
    int length;

    union {
        /* basic types */
        char* err;
        long lng;
        double dbl;
        char* sym;
        char* str;
        bool bln;

        /* dict type */
        dict* d;

        /* function types */
        struct {
            zlbuiltin builtin;
            char* builtin_name;
        };
        struct {
            zlenv* env;
            zlval* formals;
            zlval* body;
            bool called;
        };
    };
};

struct zlenv {
    zlenv* parent;
    dict* internal_dict;
    bool top_level;
    int references;
};

/* zlval instantiation functions */
zlval* zlval_err(const char* fmt, ...);
zlval* zlval_int(long x);
zlval* zlval_float(double x);
zlval* zlval_sym(const char* s);
zlval* zlval_qsym(const char* s);
zlval* zlval_str(const char* s);
zlval* zlval_bool(bool b);
zlval* zlval_fun(const zlbuiltin builtin, const char* builtin_name);
zlval* zlval_lambda(zlenv* closure, zlval* formals, zlval* body);
zlval* zlval_macro(zlenv* closure, zlval* formals, zlval* body);
zlval* zlval_dict(void);
zlval* zlval_sexpr(void);
zlval* zlval_qexpr(void);
zlval* zlval_eexpr(void);
zlval* zlval_cexpr(void);

/* zlval manipulation functions */
void zlval_del(zlval* v);
zlval* zlval_add(zlval* v, zlval* x);
zlval* zlval_add_front(zlval* v, zlval* x);

zlval* zlval_add_dict(zlval* x, zlval* k, zlval* v);
zlval* zlval_get_dict(zlval* x, zlval* k);
zlval* zlval_rm_dict(zlval* x, zlval* k);
bool zlval_haskey_dict(zlval* x, zlval* k);
zlval* zlval_keys_dict(zlval* x);
zlval* zlval_vals_dict(zlval* x);

zlval* zlval_pop(zlval* v, int i);
zlval* zlval_take(zlval* v, int i);
zlval* zlval_join(zlval* x, zlval* y);
zlval* zlval_insert(zlval* x, zlval* y, int i);
zlval* zlval_shift(zlval* x, zlval* y, int i);
zlval* zlval_reverse(zlval* x);
zlval* zlval_slice(zlval* x, int start, int end);
zlval* zlval_slice_step(zlval* x, int start, int end, int step);
void zlval_maybe_promote_numeric(zlval* a, zlval* b);
void zlval_promote_numeric(zlval* a);
void zlval_demote_numeric(zlval* a);
zlval* zlval_copy(const zlval* v);
zlval* zlval_convert(zlval_type_t t, const zlval* v);
bool zlval_eq(zlval* x, zlval* y);

/* zlval utility functions */
bool is_zlval_empty_qexpr(zlval* x);

/* zlenv functions */
zlenv* zlenv_new(void);
zlenv* zlenv_new_top_level(void);
void zlenv_del(zlenv* e);
void zlenv_del_top_level(zlenv* e);
int zlenv_index(zlenv* e, zlval* k);
zlval* zlenv_get(zlenv* e, zlval* k);
void zlenv_put(zlenv* e, zlval* k, zlval* v);
void zlenv_put_global(zlenv* e, zlval* k, zlval* v);
zlenv* zlenv_copy(zlenv* e);

void zlenv_add_builtin(zlenv* e, char* name, zlbuiltin func);
void zlenv_add_builtins(zlenv* e);

#endif