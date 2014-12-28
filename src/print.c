#include "../include/print.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../lib/mpc/mpc.h"
#include "../include/assert.h"
#include "../include/util.h"

#define BUFSIZE 4096

static void (*print_fn)(char*);

static void default_print_fn(char* s) {
    fputs(s, stdout);
}

void register_print_fn(void (*fn)(char*)) {
    print_fn = fn;
}

void register_default_print_fn(void) {
    print_fn = &default_print_fn;
}

void zl_printf(const char* format, ...) {
    char* buffer = safe_malloc(BUFSIZE);
    va_list arguments;
    va_start(arguments, format);

    vsnprintf(buffer, BUFSIZE, format, arguments);
    print_fn(buffer);

    va_end(arguments);
    free(buffer);
}

void zlval_println(const zlval* v) {
    zlval_print(v);
    print_fn("\n");
}

void zlval_print(const zlval* v) {
    char* str = zlval_to_str(v);
    print_fn(str);
    free(str);
}

static void zlval_write_sb(stringbuilder_t* sb, const zlval* v);

static void zlval_expr_print(stringbuilder_t* sb, const zlval* v, const char* open, const char* close) {
    stringbuilder_write(sb, open);
    for (int i = 0; i < v->count; i++) {
        zlval_write_sb(sb, v->cell[i]);

        if (i != (v->count - 1)) {
            stringbuilder_write(sb, " ");
        }
    }
    stringbuilder_write(sb, close);
}

static void zlval_dict_print(stringbuilder_t* sb, const dict* d) {
    stringbuilder_write(sb, "[");

    int count = dict_count(d);
    char** keys = dict_all_keys(d);
    zlval** vals = (zlval**)dict_all_vals(d);

    for (int i = 0; i < count; i++) {
        stringbuilder_write(sb, ":'%s'", keys[i]);
        stringbuilder_write(sb, " ");
        zlval_write_sb(sb, vals[i]);

        if (i != (count - 1)) {
            stringbuilder_write(sb, " ");
        }
    }

    free(keys);
    free(vals);

    stringbuilder_write(sb, "]");
}

static void zlval_print_str(stringbuilder_t* sb, const zlval* v) {
    char* escaped = safe_malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);

    escaped = mpcf_escape(escaped);
    stringbuilder_write(sb, "\"%s\"", escaped);

    free(escaped);
}

static void zlval_write_sb(stringbuilder_t* sb, const zlval* v) {
    switch (v->type) {
        case ZLVAL_ERR:
            stringbuilder_write(sb, "Error: %s", v->err);
            break;

        case ZLVAL_INT:
            stringbuilder_write(sb, "%li", v->lng);
            break;

        case ZLVAL_FLOAT:
            stringbuilder_write(sb, "%f", v->dbl);
            break;

        case ZLVAL_SYM:
            stringbuilder_write(sb, "%s", v->sym);
            break;

        case ZLVAL_QSYM:
            stringbuilder_write(sb, ":'%s'", v->sym);
            break;

        case ZLVAL_STR:
            zlval_print_str(sb, v);
            break;

        case ZLVAL_BOOL:
            if (v->bln) {
                stringbuilder_write(sb, "true");
            } else {
                stringbuilder_write(sb, "false");
            }
            break;

        case ZLVAL_BUILTIN:
            stringbuilder_write(sb, "<builtin %s>", v->builtin_name);
            break;

        case ZLVAL_FN:
            stringbuilder_write(sb, "(fn ");
            zlval_write_sb(sb, v->formals);
            stringbuilder_write(sb, " ");
            zlval_write_sb(sb, v->body);
            stringbuilder_write(sb, ")");
            break;

        case ZLVAL_MACRO:
            stringbuilder_write(sb, "(macro ");
            zlval_write_sb(sb, v->formals);
            stringbuilder_write(sb, " ");
            zlval_write_sb(sb, v->body);
            stringbuilder_write(sb, ")");
            break;

        case ZLVAL_DICT:
            zlval_dict_print(sb, v->d);
            break;

        case ZLVAL_SEXPR:
            zlval_expr_print(sb, v, "(", ")");
            break;

        case ZLVAL_QEXPR:
            zlval_expr_print(sb, v, "{", "}");
            break;

        case ZLVAL_EEXPR:
            zlval_expr_print(sb, v, "\\", "");
            break;

        case ZLVAL_CEXPR:
            zlval_expr_print(sb, v, "@", "");
            break;
    }
}

char* zlval_to_str(const zlval* v) {
    stringbuilder_t* sb = stringbuilder_new();
    zlval_write_sb(sb, v);
    char* str = stringbuilder_to_str(sb);
    stringbuilder_del(sb);
    return str;
}
