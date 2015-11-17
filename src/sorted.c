
#include "cs165_api.h"
#include "sorted.h"
#include "utils.h"

#include <string.h>

sorted_index *create_sorted_index(void) {
    sorted_index *index = malloc(sizeof(sorted_index));
    assert(index != NULL);

    index->length = 0;
    index->max_length = INITIAL_LENGTH;
    index->data = malloc(sizeof(int) * index->max_length);
    assert(index->data != NULL);
    index->positions = malloc(sizeof(size_t) * index->max_length);
    assert(index->positions != NULL);

    return index;
}

void destroy_sorted_index(sorted_index *index) {
    assert(index != NULL);
    assert(index->data != NULL);
    assert(index->positions != NULL);
    free(index->data);
    free(index->positions);
    free(index);
}

// search where n is or where n should be
size_t search_sorted(sorted_index *index, int n) {
    int ret = binary_search(index->data, n, index->length);
    if (ret >= 0)
        return ret;

    // In case binary_search didn't find anything
    if (n <= index->data[0])
        return 0;
    else
        return index->length;
}

vector *select_one_sorted(sorted_index *index, int low, int high) {
    size_t lp = search_sorted(index, low);
    size_t hp = search_sorted(index, high);

    vector *ret = create_vector(hp - lp);

    // performing the select
    for (size_t i = lp; i < hp; i++)
        vector_insert(index->positions[i], ret);
    return ret;
}


void insert_sorted(sorted_index *index, int n, size_t pos) {
    // where n will be inserted
    size_t i = search_sorted(index, n);
    if (index->length >= index->max_length) {
        index->max_length *= 2;
        index->data = realloc(index->data, index->max_length * sizeof(int));
        index->positions = realloc(index->positions, index->max_length * sizeof(size_t));
        assert(index->data != NULL);
        assert(index->positions != NULL);
    }


    memmove(&index->data[i+1], &index->data[i], (index->length - i) * sizeof(int));
    memmove(&index->positions[i+1], &index->positions[i], (index->length - i) * sizeof(size_t));

    // finally insert it
    index->data[i] = n;
    index->positions[i] = pos;
    index->length++;
}
