#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vector.h"

#define INITIAL_length 32

// Here we implement a basic int vector that grows dinamically

vector *create_vector(const char *name) {
    vector *v = malloc(sizeof(vector));
    if (name == NULL)
        v->name = NULL;
    else
        v->name = strdup(name);

    // Allocates some initial space
    v->buf = malloc(INITIAL_length * sizeof(int));
    v->length = 0;
    v->max_length = INITIAL_length;
    return v;
}

void vector_insert(int val, vector *v) {
    assert(v != NULL);
    if (v->length >= v->max_length) {
        v->max_length *= 2;
        v->buf = realloc(v->buf, v->max_length * sizeof(int));
        assert(v->buf != NULL);
    }

    // actually inserts val and increments the length
    v->buf[v->length++] = val;
}

void destroy_vector(vector *v) {
    assert(v->name != NULL);
    free(v->name);

    if (v->buf != NULL)
        free(v->buf);

    free(v);
}
