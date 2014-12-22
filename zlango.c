#include "lib/argtable/argtable3.h"

/* global arg_xxx structs */
struct arg_lit *verb, *help, *version;
struct arg_int *repl;
struct arg_file *o, *file;
struct arg_end *end;

int main(int argc, char *argv[])
{
    /* the global arg_xxx structs are initialised within the argtable */
    void *argtable[] = {
        help    = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
        version = arg_litn(NULL, "version", 0, 1, "display version info and exit"),
        repl    = arg_intn(NULL, "repl", "<n>", 0, 1, "start zlango repl"),
        verb    = arg_litn("v", "verbose", 0, 1, "verbose output"),
        o       = arg_filen("o", NULL, "myfile", 0, 1, "output file"),
        file    = arg_filen(NULL, NULL, "<file>", 1, 100, "input files"),
        end     = arg_end(20),
    };

    int exitcode = 0;
    char progname[] = "zlango";
    char ver_num[] = "0.0.1";
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
        printf("%s %s\n", progname, ver_num);
        exitcode = 0;
        goto exit;
    }

    /* case: '--repl' */
    if (repl->count > 0) {

    }

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0) {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout, end, progname);
        printf("Send us a report if you think there's a bug\n", progname);
        exitcode = 1;
        goto exit;
    }

exit:
    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
}