#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures.h"
#include "macros.h"

lenv* lenv_new(void);
lenv* lenv_copy(lenv* e);

void lenv_def(lenv* e, lval* k, lval* v);
void lenv_put(lenv* e, lval* k, lval* v);
void lenv_del(lenv* e);
