#include "include/zlango.h"
#include "include/lval.h"

/* support for Windows */
#ifdef _WIN32

/* Declare a buffer for user input of size 2048 */
static char buffer[2048];

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

/* global arg_xxx structs */
struct arg_lit *help, *version, *repl;
struct arg_file *file, *compile;
struct arg_end *end;

int main(int argc, char *argv[])
{
    /* the global arg_xxx structs are initialised within the argtable */
    void *argtable[] = {
        help    = arg_litn("h", "help", 0, 1, "display this help and exit"),
        compile = arg_filen("c", NULL, "<file>", 2, 100, "compile *.zl files"),
        repl    = arg_litn(NULL, "repl", 0, 1, "start zlango repl"),
        file    = arg_filen(NULL, NULL, "<file>", 1, 100, "run *.zl files"),
        version = arg_litn("v", "version", 0, 1, "display version info and exit"),
        end     = arg_end(20),
    };

    int exitcode = 0;
    char progname[] = "zlango";
    char zlang_ver[] = "0.0.1";
    char zlang_repl_ver[] = "0.0.1";
    int nerrors;
    nerrors = arg_parse(argc,argv,argtable);

    /* Create Some Parsers */
    Number  = mpc_new("number");
    Symbol  = mpc_new("symbol");
    String  = mpc_new("string");
    Comment = mpc_new("comment");
    Sexpr   = mpc_new("sexpr");
    Qexpr   = mpc_new("qexpr");
    Expr    = mpc_new("expr");
    Zlango  = mpc_new("zlango");
  
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                              \
      number  : /-?[0-9]+/ ;                       \
      symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; \
      string  : /\"(\\\\.|[^\"])*\"/ ;             \
      comment : /;[^\\r\\n]*/ ;                    \
      sexpr   : '(' <expr>* ')' ;                  \
      qexpr   : '{' <expr>* '}' ;                  \
      expr    : <number>  | <symbol> | <string>    \
              | <comment> | <sexpr>  | <qexpr>;    \
      zlango  : /^/ <expr>* /$/ ;                  \
    ",
    Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Zlango);

    lenv* e = lenv_new();
    lenv_add_builtins(e);

    /* case: '--help' */
    if (help->count > 0) {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        printf("\n");
        exitcode = 0;
        goto exit;
    }

    /* case: '--version' */
    if (version->count > 0) {
        printf("%s %s\n", progname, zlang_ver);
        exitcode = 0;
        goto exit;
    }

    /* case: '--repl' */
    if (repl->count > 0) {
        /* Print Version and Exit Information */
        puts(BLUE);
        puts("  ______                         ");
        puts(" |__  / | __ _ _ __   __ _  ___  ");
        puts("   / /| |/ _` | '_ \\ / _` |/ _ \\ ");
        puts("  / /_| | (_| | | | | (_| | (_) |");
        puts(" /____|_|\\__,_|_| |_|\\__, |\\___/ ");
        puts("                     |___/       ");
        puts(RESET);
        printf("Zlango repl - " RED "v%s" RESET " (Press Ctrl+c to exit)\n", zlang_repl_ver);

        /* In a never ending loop */
        while (1) {
            /* Output our prompt and get input */
            char* input = readline("zlango> ");
            
            /* Add input to history */
            add_history(input);
            
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

            /* Free retrieved input */
            free(input);
        }

        lenv_del(e);

        /* Undefine and delete our parsers */
        mpc_cleanup(8, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Zlango);

        return 0;
    }

    if (file->count >= 1) {
        for (int i = 1; i < argc; i++) {
            lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));
            lval* x = builtin_load(e, args);
            lval_println(x);
            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }

        exitcode = 0;
        goto exit;
    }

    if (compile->count >= 2) {
        for (int i = 2; i < argc; i++) {
            lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));
            lval* x = builtin_load(e, args);
            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }

        exitcode = 0;
        goto exit;
    }

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0) {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout, end, progname);
        printf("Send us a report if you think there's a bug\n");
        exitcode = 1;
        goto exit;
    }

exit:
    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
}