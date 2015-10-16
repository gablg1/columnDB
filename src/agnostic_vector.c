#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "common.h"
#include "cs165_api.h"
#include "agnostic_vector.h"
#include "variables.h"
#include "utils.h"
#include "message.h"

#define INITIAL_COUNT 32


AgnosticVector *create_agnostic_vector(size_t elem_size) {
    AgnosticVector *v = malloc(sizeof(AgnosticVector));
    v->elem_size = elem_size;

    // Allocates some initial space
    v->buf = malloc(INITIAL_COUNT * v->elem_size);
    v->length = 0;
    v->max_length = INITIAL_COUNT;
    return v;
}

void agnostic_vector_insert(void *val, AgnosticVector *v) {
    assert(v != NULL);
    if (v->length >= v->max_length) {
        v->max_length *= 2;
        v->buf = realloc(v->buf, v->max_length * v->elem_size);
        assert(v->buf != NULL);
    }

    // actually inserts val and increments the length
    void *dst = get_addr(v->length, v);
    memcpy(dst, val, v->elem_size);
    v->length++;
}

// gets address of i-th element
void *get_addr(size_t i, AgnosticVector *v) {
    assert(sizeof(uint8_t) == 1);
    return (uint8_t *) v->buf + i;
}

void destroy_agnostic_vector(AgnosticVector *v) {
    if (v->buf != NULL)
        free(v->buf);

    free(v);
}

