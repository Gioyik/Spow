#include "../include/dict.h"

#include <stdlib.h>
#include <string.h>

#include "../include/util.h"

#define DICT_INITIAL_SIZE 16
#define DICT_PROBE_INTERVAL 1
#define DICT_LOAD_FACTOR 0.75
#define DICT_GROWTH_FACTOR 2

static void* maybe_copy(const dict* d, void* v) {
    if (d->copier) {
        return d->copier(v);
    } else {
        return v;
    }
}

static void maybe_delete(const dict* d, void* v) {
    if (d->deleter) {
        d->deleter(v);
    }
}

dict* dict_new(const copy_fn copier, const del_fn deleter) {
    dict* d = dict_new_no_bindings();
    d->copier = copier;
    d->deleter = deleter;
    return d;
}

dict* dict_new_no_bindings(void) {
    dict* d = safe_malloc(sizeof(dict));
    d->size = DICT_INITIAL_SIZE;
    d->count = 0;
    d->syms = safe_malloc(sizeof(char*) * DICT_INITIAL_SIZE);
    for (int i = 0; i < DICT_INITIAL_SIZE; i++) {
        d->syms[i] = NULL;
    }
    d->vals = safe_malloc(sizeof(void*) * DICT_INITIAL_SIZE);
    d->copier = NULL;
    d->deleter = NULL;
    return d;
}

void dict_del(dict* d) {
    for (int i = 0; i < d->size; i++) {
        if (d->syms[i]) {
            free(d->syms[i]);
            maybe_delete(d, d->vals[i]);
        }
    }
    free(d->syms);
    free(d->vals);
    free(d);
}

static unsigned int dict_hash(const char* str) {
    /* djb2 hash */
    unsigned int hash = 5381;
    for (int i = 0; str[i]; i++) {
        /* XOR hash * 33 with current char val */
        hash = ((hash << 5) + hash) ^ str[i];
    }
    return hash;
}

static int dict_findslot(const dict* d, const char* k) {
    unsigned int i = dict_hash(k) % d->size;
    unsigned int probe = 1;
    while (d->syms[i] && !streq(d->syms[i], k)) {
        i = (i + probe) % d->size;
        probe += DICT_PROBE_INTERVAL;
    }
    return i;
}

static void* dict_lookup(const dict* d, const char* k) {
    int i = dict_findslot(d, k);
    if (d->syms[i]) {
        return d->vals[i];
    }

    return NULL;
}

/* forward declaration */
static void dict_resize(dict* d);

static void dict_set(dict* d, const char* k, void* v) {
    int i = dict_findslot(d, k);
    if (d->syms[i]) {
        v = maybe_copy(d, v);
        maybe_delete(d, d->vals[i]);
        d->vals[i] = v;
        return;
    }

    /* no existing entry found */
    d->count++;
    /* resize if needed */
    if (d->count / (float)d->size >= DICT_LOAD_FACTOR) {
        dict_resize(d);
        i = dict_findslot(d, k);
    }
    d->syms[i] = safe_malloc(strlen(k) + 1);
    strcpy(d->syms[i], k);
    d->vals[i] = maybe_copy(d, v);
}

static void dict_resize(dict* d) {
    int oldsize = d->size;
    d->size = d->size * DICT_GROWTH_FACTOR;

    char** syms = d->syms;
    void** vals = d->vals;

    d->syms = safe_malloc(sizeof(char*) * d->size);
    for (int i = 0; i < d->size; i++) {
        d->syms[i] = NULL;
    }
    d->vals = safe_malloc(sizeof(void*) * d->size);

    for (int i = 0; i < oldsize; i++) {
        if (syms[i]) {
            dict_set(d, syms[i], vals[i]);

            free(syms[i]);
            maybe_delete(d, vals[i]);
        }
    }
    free(syms);
    free(vals);
}

int dict_index(const dict* d, const char* k) {
    int i = dict_findslot(d, k);
    if (!d->syms[i]) {
        i = -1;
    }
    return i;
}

void* dict_get(const dict* d, const char* k) {
    return maybe_copy(d, dict_lookup(d, k));
}

void* dict_get_at(const dict* d, int i) {
    return maybe_copy(d, d->vals[i]);
}

void dict_put(dict* d, const char* k, void* v) {
    dict_set(d, k, v);
}

void dict_rm(dict* d, const char* k) {
    int i = dict_findslot(d, k);
    if (d->syms[i]) {
        d->count--;
        maybe_delete(d, d->vals[i]);
        free(d->syms[i]);
        d->syms[i] = NULL;
    }
}

dict* dict_copy(const dict* d) {
    dict* n = safe_malloc(sizeof(dict));
    n->size = d->size;
    n->count = 0;
    n->copier = d->copier;
    n->deleter = d->deleter;
    n->syms = safe_malloc(sizeof(char*) * d->size);
    for (int i = 0; i < d->size; i++) {
        n->syms[i] = NULL;
    }
    n->vals = safe_malloc(sizeof(void*) * d->size);

    for (int i = 0; i < d->size; i++) {
        if (d->syms[i]) {
            dict_set(n, d->syms[i], d->vals[i]);
        }
    }

    return n;
}

int dict_count(const dict* d) {
    return d->count;
}

char** dict_all_keys(const dict* d) {
    char** keys = safe_malloc(sizeof(char*) * d->count);
    int offset = 0;

    for (int i = 0; i < d->size; i++) {
        if (d->syms[i]) {
            keys[offset++] = d->syms[i];
        }
    }
    return keys;
}

void** dict_all_vals(const dict* d) {
    void** vals = safe_malloc(sizeof(void*) * d->count);
    int offset = 0;

    for (int i = 0; i < d->size; i++) {
        if (d->syms[i]) {
            vals[offset++] = d->vals[i];
        }
    }
    return vals;
}

bool dict_equal(const dict* d1, const dict* d2) {
    if (d1->count != d2->count) {
        return false;
    }
    /* TODO: Check keys/values, but order doesn't matter! */
    return true;
}
