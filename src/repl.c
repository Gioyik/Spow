#include "../include/repl.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "../include/zlango.h"
#include "../include/assert.h"
#include "../include/parser.h"
#include "../include/print.h"
#include "../include/eval.h"
#include "../include/util.h"

#define HIST_FILE ".zlango_history"

#if defined(_WIN32)

#include <stdio.h>
#include <string.h>

#define REPL_MAX_INPUT 2048
static char buffer[REPL_MAX_INPUT];

char* get_input(char* prompt) {
    fputs(prompt, stdout);
    fflush(stdout);
    fgets(buffer, REPL_MAX_INPUT, stdin);

    char* copy = safe_malloc(strlen(buffer) + 1);
    strcpy(copy, buffer);
    copy[strlen(copy) - 1] = '\0';
    return copy;
}

void load_history() {
}

void add_history(char* unused) {
}

#else

#include "../lib/linenoise/linenoise.h"

char* get_input(char* prompt) {
    return linenoise(prompt);
}

void load_history() {
    linenoiseHistoryLoad(HIST_FILE);
}

void add_history(char* input) {
    linenoiseHistoryAdd(input);
    linenoiseHistorySave(HIST_FILE);
}

#endif

zlval* eval_repl(zlenv* e, zlval* v) {
    if (v->count == 0) {
        zlval_del(v);
        return zlval_qexpr();
    }
    if (v->count != 1) {
        zlval_del(v);
        return zlval_err("too many expressions in REPL; only one is allowed");
    }
    return zlval_eval(e, zlval_take(v, 0));
}

void eval_repl_str(zlenv* e, const char* input) {
    zlval* v;
    char* err;
    if (zlval_parse(input, &v, &err)) {
        zlval* x = eval_repl(e, v);
        if (!is_zlval_empty_qexpr(x)) {
            zlval_println(x);
        }
        zlval_del(x);
    } else {
        zl_printf("%s", err);
        free(err);
    }
}

static void sigint_handler(int ignore) {
    zlval_eval_abort();
}

static void setup_sigint_handler(void) {
    signal(SIGINT, sigint_handler);
}

static bool repl_aborted = false;

void abort_repl(void) {
    repl_aborted = true;
}

void run_repl(zlenv* e) {
    setup_sigint_handler();
    puts(BLUE);
    puts("          _                         ");
    puts("         | |                        ");
    puts("      ___| | __ _ _ __   __ _  ___  ");
    puts("     |_  / |/ _` | '_ \\ / _` |/ _ \\ ");
    puts("      / /| | (_| | | | | (_| | (_) |");
    puts("     /___|_|\\__,_|_| |_|\\__, |\\___/ ");
    puts("                         __/ |      ");
    puts("                        |___/       ");
    puts(RESET);
    printf("Zlango repl - " RED "%s" RESET " (Press Ctrl+D to exit)\n", get_zl_version());

    load_history();

    while (!repl_aborted) {
        errno = 0;
        char* input = get_input("zlango> ");
        if (!input) {
            if (errno == EAGAIN) {
                continue;
            } else {
                zl_printf("\n");
                break;
            }
        }
        add_history(input);
        eval_repl_str(e, input);
        free(input);
    }
}
