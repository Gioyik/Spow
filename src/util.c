// To allow certain functions from unistd
#define _BSD_SOURCE 1

#include "../include/util.h"

#include <string.h>
#include <libgen.h>
#include <unistd.h>

#define BUFSIZE 4096
#define SB_START_SIZE 1024
#define SB_GROWTH_FACTOR 2

stringbuilder_t* stringbuilder_new(void) {
    stringbuilder_t* sb = safe_malloc(sizeof(stringbuilder_t));
    sb->length = 0;
    sb->size = SB_START_SIZE;
    sb->str = safe_malloc(SB_START_SIZE);
    return sb;
}

static void stringbuilder_resize(stringbuilder_t* sb) {
    sb->size = sb->size * SB_GROWTH_FACTOR;
    char* str = sb->str;

    sb->str = safe_malloc(sb->size);
    memcpy(sb->str, str, sb->length);

    free(str);
}

void stringbuilder_write(stringbuilder_t* sb, const char* format, ...) {
    va_list arguments1;
    va_list arguments2;
    va_start(arguments1, format);
    va_copy(arguments2, arguments1);

    /* First use vsnprintf to check how large the printed result would be */
    int count = vsnprintf(NULL, 0, format, arguments1);

    /* If there isn't enough room, keep resizing until there is */
    int required_size = sb->length + count + 1;
    while (sb->size < required_size) {
        stringbuilder_resize(sb);
    }

    vsnprintf((sb->str + sb->length), count + 1, format, arguments2);
    sb->length += count;

    va_end(arguments1);
    va_end(arguments2);
}

char* stringbuilder_to_str(stringbuilder_t* sb) {
    char* buffer = safe_malloc(sb->length + 1);
    memcpy(buffer, sb->str, sb->length + 1);
    return buffer;
}

void stringbuilder_del(stringbuilder_t* sb) {
    free(sb->str);
    free(sb);
}

bool streq(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}

char* strrev(const char* str) {
    int len = strlen(str);
    char* newstr = safe_malloc(len + 1);

    char* start = newstr;
    const char* end = str + len - 1;

    for (int i = 0; i < len; i++) {
        *start = *end;
        start++;
        end--;
    }
    newstr[len] = '\0';
    return newstr;
}

char* strsubstr(const char* str, int start, int end) {
    int len = end - start;
    char* buffer = safe_malloc(len + 1);
    memcpy(buffer, &str[start], len);
    buffer[len] = '\0';
    return buffer;
}

char* strstep(const char* str, int step) {
    int len = strlen(str);
    int bufferlen = len / step + (len % step == 0 ? 0 : 1);
    char* buffer = safe_malloc(bufferlen + 1);
    for (int i = 0, j = 0; i < bufferlen; i++, j += step) {
        buffer[i] = str[j];
    }
    buffer[bufferlen] = '\0';
    return buffer;
}

char* get_executable_path(void) {
    /* TODO: reading /proc is definitely not cross platform */
    char* path = safe_malloc(BUFSIZE);

    int len = readlink("/proc/self/exe", path, BUFSIZE - 1);
    if (len == -1) {
        free(path);
        path = NULL;
    } else {
        path[len] = '\0';
    }
    return path;
}

char* get_base_path(void) {
#ifdef EMSCRIPTEN
    char* base_path = safe_malloc(2);
    base_path[0] = '.';
    base_path[1] = '\0';
    return base_path;
#else
    // Executable is in 'bin' directory, so we need to go up twice
    char* exe_path = get_executable_path();
    char* base_path = path_join(dirname(exe_path), "../");
    free(exe_path);
    return base_path;
#endif
}

char* path_join(const char* a, const char* b) {
    char* buffer = safe_malloc(BUFSIZE);
    snprintf(buffer, BUFSIZE, "%s/%s", a, b);
    return buffer;
}
