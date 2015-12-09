#include <stdbool.h>
#include <cs165_api.h>

#define FANOUT 170
#define NODE_SIZE (FANOUT-1)

typedef struct record {
    size_t pos;
    data val;
} record;


/*
 * node->children[i] is the pointer with values greater than node->data[i-1]
 * and smaller than node->data[i].
 *
 * node->children[node->length] is the last pointer
 *
 * Size of bt_node struct is
 * F * 8 +  16 * (F - 1) + 16
 */
typedef struct bt_node {
    struct bt_node *parent;
	struct bt_node *children[FANOUT];
	int length;
	record records[NODE_SIZE];
	int leaf;
	struct bt_node *next;
} bt_node;

bt_node *create_btree_index(vector *v);

void bt_insert_bulk(bt_node **root, vector *v);

size_t bt_search_inner(bt_node *node, data n);

void bt_insert_non_full(bt_node *x, record n);

void bt_insert(bt_node **root, record n);

bool bt_is_full(bt_node *x);

void bt_make_room(bt_node *node, int i);


void bt_split_child(bt_node *parent, bt_node *node, size_t i);

// returns the node where n is or the leaf where n should be inserted
bt_node *bt_search_node(bt_node *root, data d);

vector *select_one_btree(bt_node *root, data low, data high);
