#include "structures.h"
#include "zlango.h"
#include "../lib/mpc/mpc.h"

lval* lval_err(char* m);
lval* lval_num(long x);
lval* lval_sym(char* s);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_add(lval* v, lval* x);
lval* lval_pop(lval* v, int i);
lval* lval_join(lval* x, lval* y);
lval* lval_take(lval* v, int i);
lval* builtin_op(lval* a, char* op);
lval* builtin(lval* a, char* func);
lval* lval_eval(lval* v);
lval* lval_eval_sexpr(lval* v);
lval* lval_eval(lval* v);
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
lval* builtin_list(lval* a);
lval* builtin_head(lval* a);
lval* builtin_tail(lval* a);
lval* builtin_eval(lval* a);
lval* builtin_join(lval* a);

void lval_del(lval* v);
void lval_print(lval* v);
void lval_expr_print(lval* v, char open, char close);
void lval_print(lval* v);
void lval_println(lval* v);

lval* builtin_op(lval* a, char* op);