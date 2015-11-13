#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "cs165_api.h"


// Here we implement a basic int vector that grows dinamically

vector *create_vector(void) {
    vector *v = malloc(sizeof(vector));
    assert(v != NULL);

    // Allocates some initial space
    v->buf = malloc(INITIAL_LENGTH * sizeof(int));
    assert(v->buf != NULL);
    v->length = 0;
    v->max_length = INITIAL_LENGTH;
    return v;
}

void vector_cat(vector *from, vector *to) {
    size_t new_length = to->length;
    if (new_length == 0)
        new_length = 64;

    while (new_length < to->length + from->length) {
        new_length *= 2;
    }

    if (new_length > to->length) {
        to->buf = realloc(to->buf, new_length * sizeof(int));
        assert(to->buf != NULL);
        to->max_length = new_length;
    }
    // after here, to has enough space to get the elements from from
    memcpy(to->buf + to->length, from->buf, from->length * sizeof(int));
    to->length += from->length;

}

// returns the position where the val was inserted
size_t vector_insert(int val, vector *v) {
    assert(v != NULL);
    if (v->length >= v->max_length) {
        v->max_length *= 2;
        v->buf = realloc(v->buf, v->max_length * sizeof(int));
        assert(v->buf != NULL);
    }

    // actually inserts val and increments the length
    v->buf[v->length++] = val;
    return v->length - 1;
}

void destroy_vector(vector *v) {
    if (v->buf != NULL)
        free(v->buf);

    free(v);
}
