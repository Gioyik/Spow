#include "../include/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../lib/mpc/mpc.h"
#include "../include/assert.h"
#include "../include/util.h"

static mpc_parser_t* Integer;
static mpc_parser_t* FPoint;
static mpc_parser_t* Number;
static mpc_parser_t* Bool;
static mpc_parser_t* String;
static mpc_parser_t* Comment;
static mpc_parser_t* Symbol;
static mpc_parser_t* QSymbol;
static mpc_parser_t* Sexpr;
static mpc_parser_t* Qexpr;
static mpc_parser_t* Dict;
static mpc_parser_t* EExpr;
static mpc_parser_t* CExpr;
static mpc_parser_t* Expr;
static mpc_parser_t* Zl;

void setup_parser(void) {
    Integer = mpc_new("integer");
    FPoint = mpc_new("fpoint");
    Number = mpc_new("number");
    Bool = mpc_new("bool");
    String = mpc_new("string");
    Comment = mpc_new("comment");
    Symbol = mpc_new("symbol");
    QSymbol = mpc_new("qsymbol");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Dict = mpc_new("dict");
    EExpr = mpc_new("eexpr");
    CExpr = mpc_new("cexpr");
    Expr = mpc_new("expr");
    Zl = mpc_new("zl");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                   \
        integer : /[+-]?[0-9]+/ ;                                           \
        fpoint  : /[+-]?[0-9]+\\.[0-9]*/ | /[+-]?[0-9]*\\.[0-9]+/ ;         \
        number  : <fpoint> | <integer> ;                                    \
        bool    : \"true\" | \"false\" ;                                    \
        string  : /\"(\\\\.|[^\"])*\"/ | /'(\\\\.|[^'])*'/ ;                \
        comment : /#[^\\r\\n]*/ ;                                           \
        symbol  : /[a-zA-Z0-9_+\\-*\\/=<>!\\?&%^$]+/ ;                      \
        qsymbol : ':' <string> | ':' <symbol> ;                             \
        sexpr   : '(' <expr>* ')' ;                                         \
        qexpr   : '{' <expr>* '}' ;                                         \
        dict    : '[' (<qsymbol> <expr>)* ']' ;                             \
        eexpr   : '\\\\' <expr> ;                                           \
        cexpr   : '@' <expr> ;                                              \
        expr    : <number> | <bool> | <string> | <symbol> | <qsymbol> |     \
                  <comment> | <sexpr> | <qexpr> | <dict> |                  \
                  <eexpr> | <cexpr> ;                                       \
        zl      : /^/ <expr>* /$/ ;                                         \
        ",
        Integer, FPoint, Number, Bool, String, Comment, Symbol, QSymbol, Sexpr, Qexpr, Dict, EExpr, CExpr, Expr, Zl);
}

void teardown_parser(void) {
    mpc_cleanup(13, Integer, FPoint, Number, Bool, String, Comment, Symbol, QSymbol, Sexpr, Qexpr, Dict, EExpr, CExpr, Expr, Zl);
}

static zlval* zlval_read(const mpc_ast_t* t);

static zlval* zlval_read_int(const mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? zlval_int(x) : zlval_err("invalid number: %s", t->contents);
}

static zlval* zlval_read_float(const mpc_ast_t* t) {
    errno = 0;
    double x = strtod(t->contents, NULL);
    return errno != ERANGE ? zlval_float(x) : zlval_err("invalid float: %s", t->contents);
}

static zlval* zlval_read_bool(const mpc_ast_t* t) {
    if (streq(t->contents, "true")) {
        return zlval_bool(true);
    }
    return zlval_bool(false);
}

static zlval* zlval_read_string(const mpc_ast_t* t) {
    t->contents[strlen(t->contents) - 1] = '\0';

    char* unescaped = safe_malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);

    unescaped = mpcf_unescape(unescaped);
    zlval* str = zlval_str(unescaped);

    free(unescaped);
    return str;
}

static zlval* zlval_read_qsym(const mpc_ast_t* t) {
    if (strstr(t->children[1]->tag, "symbol")) {
        return zlval_qsym(t->children[1]->contents);
    }

    // Handle qsym string form (:'sym')
    t->children[1]->contents[strlen(t->children[1]->contents) - 1] = '\0';

    char* unescaped = safe_malloc(strlen(t->children[1]->contents + 1) + 1);
    strcpy(unescaped, t->children[1]->contents + 1);
    unescaped = mpcf_unescape(unescaped);
    zlval* qsym = zlval_qsym(unescaped);

    free(unescaped);
    return qsym;
}

static zlval* zlval_read_sym(const mpc_ast_t* t) {
    return zlval_sym(t->contents);
}

static zlval* zlval_read_dict(const mpc_ast_t* t) {
    zlval* x = zlval_dict();

    bool new_dict_item = true;
    zlval* qsym;
    zlval* val;

    for (int i = 0; i < t->children_num; i++) {
        if (streq(t->children[i]->contents, "[")) { continue; }
        if (streq(t->children[i]->contents, "]")) { continue; }
        if (streq(t->children[i]->tag, "regex")) { continue; }
        if (strstr(t->children[i]->tag, "comment")) { continue; }

        if (new_dict_item) {
            qsym = zlval_read(t->children[i]);
            new_dict_item = false;
        } else {
            val = zlval_read(t->children[i]);
            x = zlval_add_dict(x, qsym, val);
            new_dict_item = true;

            zlval_del(qsym);
            zlval_del(val);
        }
    }

    return x;
}

static zlval* zlval_read(const mpc_ast_t* t) {
    if (strstr(t->tag, "integer")) {
        return zlval_read_int(t);
    }
    if (strstr(t->tag, "fpoint")) {
        return zlval_read_float(t);
    }
    if (strstr(t->tag, "bool")) {
        return zlval_read_bool(t);
    }
    if (strstr(t->tag, "string")) {
        return zlval_read_string(t);
    }
    if (strstr(t->tag, "qsymbol")) {
        return zlval_read_qsym(t);
    }
    if (strstr(t->tag, "symbol")) {
        return zlval_read_sym(t);
    }
    if (strstr(t->tag, "dict")) {
        return zlval_read_dict(t);
    }

    zlval* x = NULL;
    /* If root '>' */
    if (streq(t->tag, ">")) {
        x = zlval_sexpr();
    }
    else if (strstr(t->tag, "sexpr")) {
        x = zlval_sexpr();
    }
    else if (strstr(t->tag, "qexpr")) {
        x = zlval_qexpr();
    }
    else if (strstr(t->tag, "eexpr")) {
        x = zlval_eexpr();
    }
    else if (strstr(t->tag, "cexpr")) {
        x = zlval_cexpr();
    }

    for (int i = 0; i < t->children_num; i++) {
        if (streq(t->children[i]->contents, "(")) { continue; }
        if (streq(t->children[i]->contents, ")")) { continue; }
        if (streq(t->children[i]->contents, "{")) { continue; }
        if (streq(t->children[i]->contents, "}")) { continue; }
        if (streq(t->children[i]->contents, ":")) { continue; }
        if (streq(t->children[i]->contents, "\\")) { continue; }
        if (streq(t->children[i]->contents, "@")) { continue; }
        if (streq(t->children[i]->tag, "regex")) { continue; }
        if (strstr(t->children[i]->tag, "comment")) { continue; }
        x = zlval_add(x, zlval_read(t->children[i]));
    }

    return x;
}

bool zlval_parse(const char* input, zlval** v, char** err) {
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Zl, &r)) {
        *v = zlval_read(r.output);
        mpc_ast_delete(r.output);
        return true;
    } else {
        *err = mpc_err_string(r.error);
        mpc_err_delete(r.error);
        return false;
    }
}

bool zlval_parse_file(const char* file, zlval** v, char** err) {
    mpc_result_t r;
    if (mpc_parse_contents(file, Zl, &r)) {
        *v = zlval_read(r.output);
        mpc_ast_delete(r.output);
        return true;
    } else {
        *err = mpc_err_string(r.error);
        mpc_err_delete(r.error);
        return false;
    }
}
