#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>
#include "lib/argtable/argtable3.h"

/* Defined colors */
#define BLUE    "\x1b[34m"
#define RED     "\x1b[31m"
#define RESET   "\x1b[0m"

/* global arg_xxx structs */
struct arg_lit *verb, *help, *version, *repl;
struct arg_file *o, *file;
struct arg_end *end;

/* Declare a buffer for user input of size 2048 */
static char input[2048];

int main(int argc, char *argv[])
{
    /* the global arg_xxx structs are initialised within the argtable */
    void *argtable[] = {
        help    = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
        version = arg_litn(NULL, "version", 0, 1, "display version info and exit"),
        repl    = arg_litn(NULL, "repl", 0, 1, "start zlango repl"),
        verb    = arg_litn("v", "verbose", 0, 1, "verbose output"),
        o       = arg_filen("o", NULL, "myfile", 0, 1, "output file"),
        file    = arg_filen(NULL, NULL, "<file>", 1, 100, "input files"),
        end     = arg_end(20),
    };

    int exitcode = 0;
    char progname[] = "zlango";
    char zlang_ver[] = "0.0.1";
    char zlang_repl_ver[] = "0.0.1";
    int nerrors;
    nerrors = arg_parse(argc,argv,argtable);

    /* case: '--help' */
    if (help->count > 0) {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Demonstrate command-line parsing in argtable3.\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
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
            
            /* Echo input back to user */    
            printf("Repl says hello to %s\n", input);

            /* Free retrieved input */
            free(input);
        }
        return 0;
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