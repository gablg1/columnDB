// a vector used to store results
typedef struct vector {
    char *name;
    int *data;
    size_t count;
    size_t max_count;
} vector;

void destroy_vector(vector *v);

vector *create_vector(char *name);

void vector_insert(int val, vector *v);
