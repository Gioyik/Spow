#ifndef LASSERTS
#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

#define LASSERT_TYPE(builtin, args, index, expect) \
    LASSERT(args, args->cell[index]->type == expect, \
            "Function '%s' passed incorrect type for argument %i. Got %s, expected %s.",\
            builtin, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(builtin, args, num) \
    LASSERT(args, args->count == num, \
            "Function '%s' passed incorrect number for arguments. Got %i, expected %i.",\
            builtin, args->count, num)

#define LASSERT_NOT_EMPTY(builtin, args, index) \
    LASSERT(args, args->cell[index]->count != 0, \
            "Function '%s' passed {} for argument %i.",\
            builtin, index)
#endif
