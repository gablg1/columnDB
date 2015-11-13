#include "cs165_api.h"
#include "btree.h"
#include <string.h>

/*
 * Implementation based on Cormen's "Introduction to Algorithms"
 */

bt_node *create_btree_index(void) {
    bt_node *ret = malloc(sizeof(bt_node));
    assert(ret != NULL);
    ret->length = 0;
    ret->leaf = true;
    ret->next = NULL;
    return ret;
}

// returns i s.t data[i] >= n is tightest
// returns i = node->length if n > data[i] for all i
size_t bt_search_inner(bt_node *node, int n) {
	size_t i = 0;
	for (i = 0; i < node->length; i++) {
		if (node->data[i] > n)
			break;
	}
	return i;
}

void bt_insert_non_full(bt_node *x, int n) {
	assert(x->length < NODE_SIZE);

	// where the value will be inserted
	size_t i = bt_search_inner(x, n);

	if (x->leaf) {
		bt_make_room(x, i);
		x->data[i] = n;
		x->length++;
	} else {
		bt_node *y = x->children[i];
		if (bt_is_full(y)) {
			bt_split_child(x, y, i);

			// after splitting, the middle key of the child
			// comes up to x
			// Here we see which of the splitted children will get the element n
			if (x->data[i] < n)
				y = x->children[i + 1];
		}
		bt_insert_non_full(y, n);
	}

}

void bt_insert(bt_node **root, int n) {
	if (bt_is_full(*root)) {
		// allocate new root and split the old one
		bt_node *new_root = malloc(sizeof(bt_node));
		new_root->children[0] = *root;
		bt_split_child(*root, new_root, 0);

		// decide which child will get the n
		size_t i = 0;
		if (new_root->data[i] < n)
			i++;

		bt_insert_non_full(new_root->children[i], n);

		// change the root
		*root = new_root;
	}
	else {
		bt_insert_non_full(*root, n);
	}

}


bool bt_is_full(bt_node *x) {
	return x->length >= NODE_SIZE;
}

void bt_make_room(bt_node *node, size_t i) {
	if (node->length - i > 0) {
		memmove(&node->data[i + 1], &node->data[i], (node->length - i) * sizeof(int));

		// we only move the pointers as well if the node is not a leaf
		if (!node->leaf)
			memmove(&node->children[i + 1], &node->children[i], (node->length - i + 1) * sizeof(bt_node *));
	}
}


void bt_split_child(bt_node *parent, bt_node *node, size_t i) {
	size_t median_i = node->length / 2;
	int median = node->data[median_i];
	bt_node *new = malloc(sizeof(bt_node));

	new->leaf = node->leaf;
	new->length = node->length - median_i - 1;
	memcpy(&new->data[0], &node->data[median_i + 1], new->length * sizeof(int));
	memcpy(&new->children[0], &node->children[median_i + 1], (new->length + 1) * sizeof(bt_node));
	node->length = median_i;

	bt_make_room(parent, i + 1);
	parent->children[i + 1] = new;
	parent->data[i + 1] = median;
	parent->length++;
}


// returns the node where n is or the leaf where n should be inserted
bt_node *bt_search_node(bt_node *root, int n) {
	if (root->leaf)
		return root;
	size_t i = bt_search_inner(root, n);
	if (i < root->length && root->data[i] == n)
		return root;

	return bt_search_node(root->children[i], n);
}


vector *select_one_btree(bt_node *root, int low, int high) {
    bt_node *ll = bt_search_node(root, low);
    bt_node *hl = bt_search_node(root, high);
    (void) ll, hl;

    vector *ret = create_vector();
    return ret;
}
