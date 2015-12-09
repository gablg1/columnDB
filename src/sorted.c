
#include "cs165_api.h"
#include "sorted.h"
#include "utils.h"

#include <string.h>

sorted_index *create_sorted_index(vector *v) {
    sorted_index *index = malloc(sizeof(sorted_index));
    assert(index != NULL);

    index->data = duplicate_vector(v);
    index->positions = sort_vector(index->data);
    assert(index->data->length == index->positions->length);

    return index;
}

void destroy_sorted_index(sorted_index *index) {
    assert(index != NULL);
    destroy_vector(index->data);
    destroy_vector(index->positions);
    free(index);
}

vector *select_one_sorted(sorted_index *index, data low, data high) {
    // lp is the left most element >= than low
    size_t lp = binary_search_left(index->data->buf, low, index->data->length);
    // hp is the rightmost element <= than high
    size_t hp = binary_search_right(index->data->buf, high, index->data->length);

    vector *ret = create_vector(hp - lp);

    // performing the select
    for (size_t i = lp; i < hp; i++)
        vector_insert(index->positions->buf[i], ret);
    return ret;
}

size_t search_sorted(sorted_index *index, data n) {
    return binary_search_left(index->data->buf, n, index->data->length);
}

