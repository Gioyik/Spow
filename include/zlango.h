#include <stdio.h>
#include <stdlib.h>

#include "../lib/argtable/argtable3.h"
#include "../lib/mpc/mpc.h"
#include "colors.h"

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Zlango;