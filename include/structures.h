/* Create Enumeration of Possible Error Types */
/* Add SYM and SEXPR as possible lval types */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR };

typedef struct lval {
    int type;
    long num;
    
    /* Error and Symbol types have some string data */
    char* err;
    char* sym;
    
    /* Count and Pointer to a list of "lval*"; */
    int count;
    struct lval** cell;
} lval;