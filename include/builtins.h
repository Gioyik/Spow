#ifndef ZL_BUILTINS_H
#define ZL_BUILTINS_H

#include <stdbool.h>
#include "types.h"

/* language builtins */
zlval* builtin_num_op(zlenv* e, zlval* a, char* op);
zlval* builtin_add(zlenv* e, zlval* a);
zlval* builtin_sub(zlenv* e, zlval* a);
zlval* builtin_mul(zlenv* e, zlval* a);
zlval* builtin_div(zlenv* e, zlval* a);
zlval* builtin_trunc_div(zlenv* e, zlval* a);
zlval* builtin_mod(zlenv* e, zlval* a);
zlval* builtin_pow(zlenv* e, zlval* a);

zlval* builtin_ord_op(zlenv* e, zlval* a, char* op);
zlval* builtin_gt(zlenv* e, zlval* a);
zlval* builtin_gte(zlenv* e, zlval* a);
zlval* builtin_lt(zlenv* e, zlval* a);
zlval* builtin_lte(zlenv* e, zlval* a);

zlval* builtin_logic_op(zlenv* e, zlval* a, char* op);
zlval* builtin_eq(zlenv* e, zlval* a);
zlval* builtin_neq(zlenv* e, zlval* a);

zlval* builtin_bool_op(zlenv* e, zlval* a, char* op);
zlval* builtin_and(zlenv* e, zlval* a);
zlval* builtin_or(zlenv* e, zlval* a);
zlval* builtin_not(zlenv* e, zlval* a);

zlval* builtin_head(zlenv* e, zlval* a);
zlval* builtin_qhead(zlenv* e, zlval* a);
zlval* builtin_tail(zlenv* e, zlval* a);
zlval* builtin_first(zlenv* e, zlval* a);
zlval* builtin_last(zlenv* e, zlval* a);
zlval* builtin_exceptlast(zlenv* e, zlval* a);
zlval* builtin_list(zlenv* e, zlval* a);
zlval* builtin_eval(zlenv* e, zlval* a);
zlval* builtin_append(zlenv* e, zlval* a);
zlval* builtin_cons(zlenv* e, zlval* a);
zlval* builtin_dictget(zlenv* e, zlval* a);
zlval* builtin_dictset(zlenv* e, zlval* a);
zlval* builtin_dictdel(zlenv* e, zlval* a);
zlval* builtin_dicthaskey(zlenv* e, zlval* a);
zlval* builtin_dictkeys(zlenv* e, zlval* a);
zlval* builtin_dictvals(zlenv* e, zlval* a);

zlval* builtin_len(zlenv* e, zlval* a);
zlval* builtin_reverse(zlenv* e, zlval* a);
zlval* builtin_slice(zlenv* e, zlval* a);

zlval* builtin_if(zlenv* e, zlval* a);
zlval* builtin_var(zlenv* e, zlval* a, bool global);
zlval* builtin_define(zlenv* e, zlval* a);
zlval* builtin_global(zlenv* e, zlval* a);
zlval* builtin_let(zlenv* e, zlval* a);

zlval* builtin_lambda(zlenv* e, zlval* a);
zlval* builtin_macro(zlenv* e, zlval* a);

zlval* builtin_typeof(zlenv* e, zlval* a);
zlval* builtin_convert(zlenv* e, zlval* a);
zlval* builtin_import(zlenv* e, zlval* a);
zlval* builtin_print(zlenv* e, zlval* a);
zlval* builtin_println(zlenv* e, zlval* a);
zlval* builtin_random(zlenv* e, zlval* a);
zlval* builtin_error(zlenv* e, zlval* a);
zlval* builtin_exit(zlenv* e, zlval* a);

#endif
