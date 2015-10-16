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

typedef struct AgnosticVector {
    size_t elem_size;
    size_t length;
    size_t max_length;
    void *buf;
} AgnosticVector;

#define INITIAL_COUNT 32


agnostic_vector *create_agnostic_vector(size_t elem_size) {
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
    if (v->count >= v->max_count) {
        v->max_count *= 2;
        v->buf = realloc(v->buf, v->max_count * v->elem_size);
        assert(v->buf != NULL);
    }

    // actually inserts val and increments the length
    void *dst = get_addr(v->count, v);
    memcpy(dst, val, elem_size);
    v->count++;
}

// gets address of i-th element
void *get_addr(size_t i, AgnosticVector *v) {
    assert(sizeof(uint_8t) == 1);
    return (uint_8t *) v->buf + i;
}

void destroy_agnostic_vector(AgnosticVector *v) {
    if (v->data != NULL)
        free(v->data);

    free(v);
}

