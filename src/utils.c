#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "utils.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#define TO_ALLOCATE 64

// buf ends with a newline character, so transform that to null
void newline_to_null(char *buf) {
    char *p = buf;
    strsep(&p, "\n");
}

void *get_next_allocated_element(size_t *count, size_t *max_count, size_t size, void *b) {
    void **buf = b;
    assert(buf != NULL);
    assert(count != NULL && max_count != NULL);
    if (*count >= *max_count) {
        *max_count += TO_ALLOCATE;
        *buf = realloc(*buf, *max_count * size);
        assert(*buf != NULL);
    }

    // returns the address of the new added element
    // if sizeof(char) != 1 the whole world breaks
    assert(sizeof(char) == 1);
    return (char *) *buf + size * ((*count)++);
}


void cs165_log(FILE* out, const char *format, ...) {
#ifdef LOG
    va_list v;
    va_start(v, format);
    vfprintf(out, format, v);
    va_end(v);
#else
    (void) out;
    (void) format;
#endif
}

void log_err(const char *format, ...) {
#ifdef LOG_ERR
    va_list v;
    va_start(v, format);
    fprintf(stderr, ANSI_COLOR_RED);
    vfprintf(stderr, format, v);
    fprintf(stderr, ANSI_COLOR_RESET);
    va_end(v);
#else
    (void) format;
#endif
}

void log_info(const char *format, ...) {
#ifdef LOG_INFO
    va_list v;
    va_start(v, format);
    fprintf(stdout, ANSI_COLOR_GREEN);
    vfprintf(stdout, format, v);
    fprintf(stdout, ANSI_COLOR_RESET);
    fflush(stdout);
    va_end(v);
#else
    (void) format;
#endif
}

