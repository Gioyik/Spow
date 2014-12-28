#ifndef ZL_DICT_H
#define ZL_DICT_H

#include <stdbool.h>

typedef void*(*copy_fn)(const void*);
typedef void(*del_fn)(void*);

typedef struct dict {
    int size;
    int count;
    char** syms;
    void** vals;
    copy_fn copier;
    del_fn deleter;
} dict;

dict* dict_new(const copy_fn copier, const del_fn deleter);
dict* dict_new_no_bindings(void);
void dict_del(dict* d);
int dict_index(const dict* d, const char* k);
void* dict_get(const dict* d, const char* k);
void* dict_get_at(const dict* d, int i);
void dict_put(dict* d, const char* k, void* v);
void dict_rm(dict* d, const char* k);
dict* dict_copy(const dict* d);
int dict_count(const dict* d);
char** dict_all_keys(const dict* d);
void** dict_all_vals(const dict* d);
bool dict_equal(const dict* d1, const dict* d2);

#endif
