#include "include/zlango.h"
#include "include/builtins.h"

void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        /* put builtin functions here ... */
        //linenoiseAddCompletion(lc,"like this");
    }
}

char* input; 
char historypath[512];

int main(int argc, char** argv)
{
    /* Create Some Parsers */
    Range    = mpc_new("range");
    Decimal  = mpc_new("decimal");
    Number   = mpc_new("number");
    String   = mpc_new("string");
    Symbol   = mpc_new("symbol");
    Comment  = mpc_new("comment");
    Sexpr    = mpc_new("sexpr");
    Qexpr    = mpc_new("qexpr");
    Expr     = mpc_new("expr");
    Zlango   = mpc_new("zlango");
  
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                             \
        range    : /-?[a-zA-Z0-9]+\\.\\.-?[a-zA-Z0-9]+/;          \
        decimal  : /-?[0-9]+\\.[0-9]+/;                           \
        number   : /-?[0-9]+/ ;                                   \
        string   : /\"(\\\\.|[^\"])*\"/ | /\'(\\\\.|[^\'])*\'/ ;                         \
        symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&|%^]+/ ;          \
        comment  : /#[^\\r\\n]*/;                              \
        sexpr    : '(' <expr>* ')' ;                              \
        qexpr    : '{' <expr>* '}' ;                              \
        expr     : <range> | <decimal> | <number> | <string> |    \
                   <symbol> | <comment> | <sexpr> | <qexpr> ;     \
        zlango    : /^/ <expr>* /$/ ;                             \
    ",
    Range, Decimal, Number, String, Symbol, Comment, Sexpr, Qexpr, Expr, Zlango);

    lenv* e = lenv_new();
    lenv_add_builtins(e);

    lval* stdlib_file = lval_add(lval_sexpr(), lval_str("syntax/core.zl"));
    lval* stdlib_load = builtin_load(e, stdlib_file);
    if (stdlib_load->type == LVAL_ERR) {
        lval_println(stdlib_load);
    }
    lval_del(stdlib_load);

    if (argc == 1) {
        /* Print Version and Exit Information */
        puts(BLUE);
        puts("  ______                         ");
        puts(" |__  / | __ _ _ __   __ _  ___  ");
        puts("   / /| |/ _` | '_ \\ / _` |/ _ \\ ");
        puts("  / /_| | (_| | | | | (_| | (_) |");
        puts(" /____|_|\\__,_|_| |_|\\__, |\\___/ ");
        puts("                     |___/       ");
        puts(RESET);
        printf("Zlango repl - " RED "v%s" RESET " (Press Ctrl+c or type exit to finish)\n", ZLANGO_VERSION);

        linenoiseSetMultiLine(1);
        linenoiseSetCompletionCallback(completion);
        sprintf(historypath, "%s/.zlango_history", getenv("HOME"));
        linenoiseHistoryLoad(historypath);

        /* Output our prompt and get input */
        while ((input = linenoise("zlango> ")) != NULL) {
            if (input[0] != '\0') {
                linenoiseHistoryAdd(input);
                linenoiseHistorySave(historypath);
                /* Attempt to parse the user input */
                mpc_result_t r;
                if (mpc_parse("<stdin>", input, Zlango, &r)) {
                    lval* x = lval_eval(e, lval_read(r.output));
                    lval_println(x);
                    lval_del(x);
                    mpc_ast_delete(r.output);
                } else {
                    /* Otherwise print and delete the Error */
                    mpc_err_print(r.error);
                    mpc_err_delete(r.error);
                }
            } else if (!strncmp(input, "/historylen", 11)) {
                int len = atoi(input+11);
                linenoiseHistorySetMaxLen(len);
            }
            /* Free retrieved input */
            free(input);
        }
    }

    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));

            lval* x = builtin_load(e, args);

            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }
    }

    lenv_del(e);

    /* Undefine and delete our parsers */
    mpc_cleanup(10, Range, Decimal, Number, String, Symbol, Comment, Sexpr, Qexpr, Expr, Zlango);

    return 0;
}