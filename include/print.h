#ifndef ZL_PRINT_H
#define ZL_PRINT_H

#include "types.h"
#include <stdarg.h>

/* printing functions */
void zlval_println(const zlval* v);
void zlval_print(const zlval* v);
void register_print_fn(void (*fn)(char*));
void register_default_print_fn(void);
void zl_printf(const char* format, ...);
char* zlval_to_str(const zlval* v);

#endif
