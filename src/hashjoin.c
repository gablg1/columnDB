#include "cs165_api.h"
#include "hashjoin.h"

#define HASH_TABLE_SIZE 128000

ht_node *hash_table[HASH_TABLE_SIZE];


int hash(data n) {
    return n % HASH_TABLE_SIZE;
}

void add_to_ht(int pos, data val) {
    int bucket = hash(val);
    ht_node *new = malloc(sizeof(ht_node));
    new->pos = pos;
    new->val = val;
    new->next = hash_table[bucket];

    hash_table[bucket] = new;
}

void free_bucket(ht_node *n) {
    if (n == NULL)
        return;
    free_bucket(n->next);
    free(n);
}

void destroy_ht(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        free_bucket(hash_table[i]);
        hash_table[i] = NULL;
    }
}

void hashjoin(vector **r1, vector **r2, vector *pos1, vector *val1, vector *pos2, vector *val2) {
    *r1 = create_vector(0);
    *r2 = create_vector(0);

    // Assumes pos1->length < pos2->length
    for (size_t i = 0; i < pos1->length; i++)
        add_to_ht(pos1->buf[i], val1->buf[i]);

    for (size_t i = 0; i < pos2->length; i++) {
        int bucket = hash(val2->buf[i]);

        for (ht_node *cur = hash_table[bucket]; cur != NULL; cur = cur->next) {
            vector_insert(cur->pos, *r1);
            vector_insert(pos2->buf[i], *r2);
        }
    }

    destroy_ht();
}


