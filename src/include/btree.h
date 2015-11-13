#include <stdbool.h>

#define FANOUT 10
#define NODE_SIZE (FANOUT-1)
/*
 * node->children[i] is the pointer with values greater than node->data[i-1]
 * and smaller than node->data[i].
 *
 * node->children[node->length] is the last pointer
 */
typedef struct bt_node {
	struct bt_node *children[FANOUT];
	size_t length;
	int data[NODE_SIZE];
	size_t positions[NODE_SIZE];
	bool leaf;
	struct bt_node *next;
} bt_node;

bt_node *create_btree_index(void);

size_t bt_search_inner(bt_node *node, int n);

void bt_insert_non_full(bt_node *x, int n);

void bt_insert(bt_node **root, int n);

bool bt_is_full(bt_node *x);

void bt_make_room(bt_node *node, size_t i);


void bt_split_child(bt_node *parent, bt_node *node, size_t i);

// returns the node where n is or the leaf where n should be inserted
bt_node *bt_search_node(bt_node *root, int n);

vector *select_one_btree(bt_node *root, int low, int high);
