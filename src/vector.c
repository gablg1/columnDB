#include <stdlib.h>
#include <string.h>

#include "vector.h"

#define INITIAL_COUNT 32

// Here we implement a basic int vector that grows dinamically

vector *create_vector(const char *name) {
    vector *v = malloc(sizeof(vector));
    if (name == NULL)
        v->name = NULL;
    else
        v->name = strdup(name);

    // Allocates some initial space
    v->buf = malloc(INITIAL_COUNT * sizeof(int));
    v->count = 0;
    v->max_count = INITIAL_COUNT;
}

void vector_insert(int val, vector *v) {
    assert(v != NULL);
    if (v->count >= v->max_count) {
        v->max_count *= 2;
        v->buf = realloc(v->buf, v->max_count * size);
        assert(v->buf != NULL);
    }

    // actually inserts val and increments the length
    v->buf[v->count++] = val;
}

void destroy_vector(vector *v) {
    assert(v->name != NULL);
    free(v->name);

    if (v->data != NULL)
        free(v->data);

    free(v);
}
