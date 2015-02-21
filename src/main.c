#include "../include/spow.h"
#include "../include/repl.h"

#ifndef EMSCRIPTEN

int main(int argc, char** argv) {
    setup_zl();
    zlenv* e = zlenv_new_top_level();

    /* if the only argument is the interpreter name, run repl */
    if (argc == 1) {
        run_repl(e);
    } else {
        run_scripts(e, argc, argv);
    }

    zlenv_del_top_level(e);
    teardown_zl();
    return 0;
}

#endif
