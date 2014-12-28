#ifndef ZL_UTIL_H
#define ZL_UTIL_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static inline void* safe_malloc(size_t size) {
    char* p = malloc(size);
    if (p == NULL) {
        fprintf(stderr, "failed to allocate memory\n");
        exit(-1);
    }
    return p;
}

typedef struct {
    int length;
    int size;
    char* str;
} stringbuilder_t;

stringbuilder_t* stringbuilder_new(void);
void stringbuilder_write(stringbuilder_t* sb, const char* format, ...);
char* stringbuilder_to_str(stringbuilder_t* sb);
void stringbuilder_del(stringbuilder_t* sb);

bool streq(const char* a, const char* b);
char* strrev(const char* str);
char* strsubstr(const char* str, int start, int end);
char* strstep(const char* str, int step);
char* get_executable_path(void);
char* get_base_path(void);
char* path_join(const char* a, const char* b);

#endif
