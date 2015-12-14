
typedef struct ht_node {
    struct ht_node *next;
    int pos;
    data val;
} ht_node;

void hashjoin(vector **r1, vector **r2, vector *pos1, vector *val1, vector *pos2, vector *val2);
