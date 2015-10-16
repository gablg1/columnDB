
typedef struct AgnosticVector {
    size_t elem_size;
    size_t length;
    size_t max_length;
    void *buf;
} AgnosticVector;

AgnosticVector *create_agnostic_vector(size_t elem_size);

void agnostic_vector_insert(void *val, AgnosticVector *v);

void *get_addr(size_t i, AgnosticVector *v);

void destroy_agnostic_vector(AgnosticVector *v);

