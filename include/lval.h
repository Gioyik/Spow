#include "structures.h"
#include "macros.h"
#include "lenv.h"
#include "zlango.h"
#include "../lib/mpc/mpc.h"

lval* lval_fun(lbuiltin func);
lval* lval_lambda(lval* formals, lval* body);

lval* lval_qexpr(void);
lval* lval_range(long x, long y);
lval* lval_sexpr(void);
lval* lval_sym(char* s);
lval* lval_str(char* s);
lval* lval_err(char* fmt, ...);
lval* lval_dec(double x);
lval* lval_num(long x);
lval* lval_bool(int val);

lval* lval_add(lval* v, lval* x);
lval* lval_eval(lenv* e, lval* v);
lval* lval_eval_sexpr(lenv*e, lval* v);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* lval_join(lval* x, lval* y);
lval* lval_copy(lval* v);
lval* lval_call(lenv* e, lval* f, lval* a);

lval* lval_read_num(mpc_ast_t* t);
lval* lval_read_dec(mpc_ast_t* t);
lval* lval_read_range(mpc_ast_t* t);
lval* lval_read_str(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);

lval* lenv_get(lenv* e, lval* k);

int lval_eq(lval* x, lval* y);

void lval_del(lval* v);
void lval_print(lval* v);
void lval_println(lval* v);
void lval_print_str(lval* v);

void lval_expr_print(lval* v, char open, char close);
char* ltype_name(int t);
