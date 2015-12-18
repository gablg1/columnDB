#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "utils.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//#define LOG
//#define LOG_ERR
//#define LOG_INFO

#define TO_ALLOCATE 64

#include <sys/time.h>

/**
 *  * Returns the current time in microseconds.
 *   */
long get_microtime(){
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

// buf ends with a newline character, so transform that to null
void newline_to_null(char *buf) {
    char *p = buf;
    strsep(&p, "\n");
}

// Adapted from the "Bit twidling hacks" http://graphics.stanford.edu/~seander/bithacks.html
unsigned long upper_power_of_two(unsigned long v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;

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

// Inspired on http://www.programmingsimplified.com/c/source-code/c-program-binary-search
// binary_search returns an int instead of a size_t because we want to indicate -1 for elements not
// in array
int binary_search(data *buf, data n, size_t length) {
    int first = 0;
    int last = length - 1;
    int mid = (first + last) / 2;
    while (first <= last) {
        if (buf[mid] < n)
            first = mid + 1;
        else if (buf[mid] == n)
            return mid;
        else
            last = mid - 1;
        mid = (first + last) / 2;
    }
    // in case we didn't find it
    return -1;
}

// returns where n should be if it were to be inserted on the left as opposed to
// a negative number
size_t binary_search_left(data *buf, data n, size_t length) {
    int first = 0;
    int last = length - 1;
    int mid = (first + last) / 2;
    while (first <= last) {
        if (buf[mid] >= n)
            last = mid - 1;
        else
            first = mid + 1;
        mid = (first + last) / 2;
    }
    return first;
}

// same as above but on the right
size_t binary_search_right(data *buf, data n, size_t length) {
    int first = 0;
    int last = length - 1;
    int mid = (first + last) / 2;
    while (first <= last) {
        if (buf[mid] > n)
            last = mid - 1;
        else
            first = mid + 1;
        mid = (first + last) / 2;
    }
    return first;
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

