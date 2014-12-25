#include <string.h>
#include <math.h>
#include <time.h>
#include "structures.h"
#include "func.h"
#include "macros.h"
#include "zlango.h"
#include "lval.h"
#include "../lib/mpc/mpc.h"

lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_cons(lenv* e, lval* a);
lval* builtin_take(lenv* e, lval* a);
lval* builtin_drop(lenv* e, lval* a);
lval* builtin_rev(lenv* e, lval* a);
lval* builtin_sort(lenv* e, lval* a);
lval* builtin_last(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);
lval* builtin_len(lenv* e, lval* a);
lval* builtin_init(lenv* e, lval* a);
lval* builtin_sum(lenv* e, lval* a);
lval* builtin_prod(lenv* e, lval* a);
lval* builtin_nth(lenv* e, lval* a);
lval* builtin_lambda(lenv* e, lval* a);
lval* builtin_op(lenv* e, lval* a, char* op);
lval* builtin_ord(lenv* e, lval* a, char* op);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_ge(lenv* e, lval* a);
lval* builtin_le(lenv* e, lval* a);
lval* builtin_cmp(lenv* e, lval* a, char* op);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_ne(lenv* e, lval* a);
lval* builtin_and(lenv* e, lval* a);
lval* builtin_or(lenv* e, lval* a);
lval* builtin_not(lenv* e, lval* a);
lval* builtin_if(lenv* e, lval* a);
lval* builtin_load(lenv* e, lval* a);
lval* builtin_print(lenv* e, lval* a);
lval* builtin_exit(lenv* e, lval* a);
lval* builtin_env(lenv* e, lval* a);
lval* builtin_error(lenv* e, lval* a);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_put(lenv* e, lval* a);
lval* builtin_inc(lenv* e, lval* a);
lval* builtin_dec(lenv* e, lval* a);
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);
lval* builtin_mod(lenv* e, lval* a);
lval* builtin_pow(lenv* e, lval* a);
lval* builtin_min(lenv* e, lval* a);
lval* builtin_max(lenv* e, lval* a);
lval* builtin_xor(lenv* e, lval* a);
lval* builtin_ln(lenv* e, lval* a);
lval* builtin_ceil(lenv* e, lval* a);
lval* builtin_floor(lenv* e, lval* a);
lval* builtin_rand(lenv* e, lval* a);
lval* builtin_leftshift(lenv* e, lval* a);
lval* builtin_rightshift(lenv* e, lval* a);
lval* builtin_bitwiseand(lenv* e, lval* a);
lval* builtin_bitwiseor(lenv* e, lval* a);
lval* builtin_bitwisexor(lenv* e, lval* a);
void lenv_add_builtin(lenv* e, char* name, lbuiltin func);
void lenv_add_builtin_var(lenv* e, char* name, lval* val);
void lenv_add_builtins(lenv* e);