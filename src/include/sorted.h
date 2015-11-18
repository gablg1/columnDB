
typedef struct sorted_index {
	size_t length;
	size_t max_length;
	data *data;
	size_t *positions;
} sorted_index;

sorted_index *create_sorted_index(void);

size_t search_sorted(sorted_index *index, data n);

vector *select_one_sorted(sorted_index *index, data low, data high);

void insert_sorted(sorted_index *index, data n, size_t pos);
