#include "cs165_api.h"

// a vector used to store results
typedef struct vector {
    data *buf;
    size_t length;
    size_t max_length;
} vector;

void destroy_vector(vector *v);

vector *create_vector(size_t length);

void vector_cat(vector *from, vector *to);

size_t vector_insert(data val, vector *v);

void sort_vector_from_positions(vector **vp, vector *positions);

vector *sort_vector(vector *v);
