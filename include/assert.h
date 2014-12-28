#ifndef ZL_ASSERT_H
#define ZL_ASSERT_H

#define ZLASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        zlval* err = zlval_err(fmt, __VA_ARGS__); \
        zlval_del(args); \
        return err; \
    }

#define ZLASSERT_TYPE(args, i, expected, fname) \
    ZLASSERT(args, (args->cell[i]->type == expected), \
            "function '%s' passed incorrect type for arg %i; got %s, expected %s", \
            fname, i, zlval_type_name(args->cell[i]->type), zlval_type_name(expected));

#define ZLASSERT_ISNUMERIC(args, i, fname) \
    ZLASSERT(args, (ISNUMERIC(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected numeric type", \
            fname, i, zlval_type_name(args->cell[i]->type));

#define ZLASSERT_ISORDEREDCOLLECTION(args, i, fname) \
    ZLASSERT(args, (ISORDEREDCOLLECTION(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected ordered collection type", \
            fname, i, zlval_type_name(args->cell[i]->type));

#define ZLASSERT_ISCOLLECTION(args, i, fname) \
    ZLASSERT(args, (ISCOLLECTION(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected collection type", \
            fname, i, zlval_type_name(args->cell[i]->type));

#define ZLASSERT_ISEXPR(args, i, fname) \
    ZLASSERT(args, (ISEXPR(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected expression type", \
            fname, i, zlval_type_name(args->cell[i]->type));

#define ZLASSERT_ARGCOUNT(args, expected, fname) \
    ZLASSERT(args, (args->count == expected), \
            "function '%s' takes exactly %i argument(s); %i given", fname, expected, args->count);

#define ZLASSERT_MINARGCOUNT(args, min, fname) \
    ZLASSERT(args, (args->count >= min), \
            "function '%s' takes %i or more arguments; %i given", fname, min, args->count);

#define ZLASSERT_RANGEARGCOUNT(args, min, max, fname) \
    ZLASSERT(args, (args->count >= min && args->count <= max), \
            "function '%s' takes between %i and %i arguments; %i given", fname, min, max, args->count);

#define ZLASSERT_NONEMPTY(args, zlval, fname) \
    ZLASSERT(args, (zlval->count != 0), "function '%s' passed {}", fname);

#define ZLASSERT_NONZERO(args, val, fname) \
    ZLASSERT(args, (val != 0), "function '%s' requires nonzero argument", fname);

#endif
