#ifndef ZLANGO_STRUCTURES
#define ZLANGO_STRUCTURES

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);

enum { LVAL_ERR, LVAL_NUM, LVAL_DEC, LVAL_SYM,
    LVAL_STR, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR,
    LVAL_BOOL};

struct lval
{
    int type;
    int is_builtin;

    /* Basics */
    long num;
    double decimal;
    char* err;
    char* sym;
    char* str;
    int bool;

    /* Functions */
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;

    /* Expressions */
    int count;
    lval** cell;
};

struct lenv
{
    lenv* par;
    int count;
    char** syms;
    lval** vals;
};
#endif
