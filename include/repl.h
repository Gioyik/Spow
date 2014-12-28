#ifndef ZL_REPL_H
#define ZL_REPL_H

#include "types.h"

zlval* eval_repl(zlenv* e, zlval* v);
void eval_repl_str(zlenv* e, const char* input);
void abort_repl(void);
void run_repl(zlenv* e);

#endif
