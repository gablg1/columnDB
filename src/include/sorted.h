
typedef struct sorted_index {
	size_t length;
	size_t max_length;
	int *data;
	size_t *positions;
} sorted_index;

sorted_index *create_sorted_index(void);

size_t search_sorted(sorted_index *index, int n);

vector *select_one_sorted(sorted_index *index, int low, int high);

void insert_sorted(sorted_index *index, int n, size_t pos);
