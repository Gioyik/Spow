#include "../include/zlango.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "../include/assert.h"
#include "../include/builtins.h"
#include "../include/parser.h"
#include "../include/print.h"
#include "../include/util.h"

void run_scripts(zlenv* e, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        zlval* args = zlval_add(zlval_sexpr(), zlval_str(argv[i]));
        zlval* x = builtin_import(e, args);

        if (x->type == ZLVAL_ERR) {
            zlval_println(x);
        }
        zlval_del(x);
    }
}

void setup_zl(void) {
    srand(time(NULL));
    register_default_print_fn();
    setup_parser();
}

void teardown_zl(void) {
    teardown_parser();
}

char* get_zl_version(void) {
    return ZLANGO_VERSION;
}
