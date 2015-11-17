// a vector used to store results
typedef struct vector {
    int *buf;
    size_t length;
    size_t max_length;
} vector;

void destroy_vector(vector *v);

vector *create_vector(size_t length);

void vector_cat(vector *from, vector *to);

size_t vector_insert(int val, vector *v);

void sort_vector_from_positions(vector **vp, vector *positions);

vector *sort_vector(vector *v);
