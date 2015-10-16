// a vector used to store results
typedef struct vector {
    char *name;
    int *buf;
    size_t length;
    size_t max_length;
} vector;

void destroy_vector(vector *v);

vector *create_vector(const char *name);

void vector_insert(int val, vector *v);
