#include <stdio.h>
#include <stdlib.h>

#include "../lib/linenoise/linenoise.h"
#include "macros.h"
#include "../lib/mpc/mpc.h"
#include "colors.h"

#define LANG_NAME "zlango"
#define ZLANGO_VERSION "0.5.0"

mpc_parser_t* Number;
mpc_parser_t* Range;
mpc_parser_t* Decimal;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Zlango;
