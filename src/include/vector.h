// a vector used to store results
typedef struct vector {
    int *buf;
    size_t length;
    size_t max_length;
} vector;

void destroy_vector(vector *v);

vector *create_vector(void);

void vector_cat(vector *from, vector *to);

size_t vector_insert(int val, vector *v);
