#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "cs165_api.h"
#include "utils.h"


// Here we implement a basic data vector that grows dinamically

vector *create_vector(size_t length) {
    vector *v = malloc(sizeof(vector));
    assert(v != NULL);
    if (length < INITIAL_LENGTH)
        length = INITIAL_LENGTH;
    else
        length = upper_power_of_two(length);

    // Allocates some initial space
    v->buf = malloc(length * sizeof(data));
    assert(v->buf != NULL);
    v->length = 0;
    v->max_length = length;
    return v;
}

vector *duplicate_vector(vector *v) {
    vector *ret = create_vector(v->length);
    vector_cat(ret, v);
    assert(ret->length == v->length);
    if (ret->length > 0)
        assert(ret->buf[0] == v->buf[0]);
    return ret;
}

void vector_cat(vector *to, vector *from) {
    size_t new_length = to->length;
    if (new_length == 0)
        new_length = 64;

    while (new_length < to->length + from->length) {
        new_length *= 2;
    }

    if (new_length > to->length) {
        to->buf = realloc(to->buf, new_length * sizeof(data));
        assert(to->buf != NULL);
        to->max_length = new_length;
    }
    // after here, to has enough space to get the elements from from
    memcpy(to->buf + to->length, from->buf, from->length * sizeof(data));
    to->length += from->length;

}

// returns the position where the val was inserted
size_t vector_insert(data val, vector *v) {
    assert(v != NULL);
    if (v->length >= v->max_length) {
        v->max_length *= 2;
        v->buf = realloc(v->buf, v->max_length * sizeof(data));
        assert(v->buf != NULL);
    }

    // actually inserts val and increments the length
    v->buf[v->length++] = val;
    return v->length - 1;
}


int compare_data(const void *a, const void *b) {
    if (*(data *)a < *(data *)b)
        return -1;
    else if (*(data *)a == *(data *)b)
        return 0;
    else
        return 1;
}


int vector_binary_search(vector *v, data n) {
    return binary_search(v->buf, n, v->length);
}

// positions->buf[i] = j indicates that you can find sorted[i] in unsorted[j]
void sort_vector_from_positions(vector **vp, vector *positions) {
    vector *v = *vp;
    assert(v->length == positions->length);
    vector *ret = create_vector(v->length);
    for (size_t i = 0; i < v->length; i++) {
        size_t j = positions->buf[i];
        ret->buf[i] = v->buf[j];
    }
    ret->length = v->length;
    destroy_vector(*vp);
    *vp = ret;
}

// returns a vector with the mapping of unsorted => sorted
// ret->buf[i] = j indicates that you can find sorted[i] in unsorted[j]
vector *sort_vector(vector *v) {
    vector *tmp = duplicate_vector(v);
    vector *ret = duplicate_vector(v);

    qsort(v->buf, v->length, sizeof(data), compare_data);

    for (size_t j = 0; j < ret->length; j++) {
        int pos = vector_binary_search(v, tmp->buf[j]);
        assert(pos >= 0);
        ret->buf[pos] = j;
    }
    destroy_vector(tmp);

    return ret;
}

void destroy_vector(vector *v) {
    assert(v != NULL);
    if (v->buf != NULL)
        free(v->buf);

    free(v);
}
