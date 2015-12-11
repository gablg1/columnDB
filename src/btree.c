#include "cs165_api.h"
#include "btree.h"
#include "utils.h"
#include <string.h>

/*
 * Implementation based on Cormen's "Introduction to Algorithms"
 */

void assert_increasing(bt_node *n) {
    for (int i = 0; i < n->length - 1; i++) {
        assert(n->records[i].val <= n->records[i+1].val);
    }
}

bt_node *create_bt_node(bool leaf) {
	bt_node *new = malloc(sizeof(bt_node));
	assert(new != NULL);
	new->length = 0;
	new->next = NULL;
	new->leaf = leaf;
	new->parent = NULL;
	return new;
}

bt_node *create_btree_index(vector *v) {
    bt_node *ret = create_bt_node(true);
    log_info("Size of btree node is: %d.\n", sizeof(bt_node));

    bt_insert_bulk(&ret, v);
    return ret;
}

void bt_insert_bulk(bt_node **root, vector *v) {
    for (size_t i = 0; i < v->length; i++) {
        record r;
        r.pos = i;
        r.val = v->buf[i];
        bt_insert(root, r);
    }
}

// returns i s.t record[i] >= n is tightest
// returns i = node->length if n > record[i] for all i
size_t bt_search_inner(bt_node *node, data d) {
	int i;
	for (i = 0; i < node->length; i++) {
		if (node->records[i].val > d)
			break;
	}

    return i;
}

void bt_insert_non_full(bt_node *x, record n) {
	assert(x->length < NODE_SIZE);

	// where the value will be inserted
	size_t i = bt_search_inner(x, n.val);

	if (x->leaf) {
		bt_make_room(x, i);
		x->records[i] = n;
		x->length++;
	} else {
	    // in this case, the lowest value of our child will change
	    // so we update the first record of x to keep our main invariant
	    if (i == 0)
	        x->records[0] = n;
        else
	        i--;

		bt_node *y = x->children[i];
		if (bt_is_full(y)) {
			bt_split_child(x, y, i + 1);

			// after splitting, the middle key of the child
			// comes up to x
			// Here we see which of the splitted children will get the element n
			if (x->records[i + 1].val < n.val)
				y = x->children[i + 1];
		}
		bt_insert_non_full(y, n);
	}

}


void bt_insert(bt_node **root, record n) {
	if (bt_is_full(*root)) {
		// allocate new root and split the old one
		bt_node *new_root = create_bt_node(false);
		new_root->children[0] = *root;
		bt_split_child(new_root, *root, 1);

		// we keep the invariant that node->children[i] points
		// to a node whose first records is node->records[i]
		new_root->records[0] = new_root->children[0]->records[0];
		new_root->length++;


		// decide which child will get the n
		size_t i = 0;
		if (new_root->records[i + 1].val < n.val)
			i++;

		bt_insert_non_full(new_root->children[i], n);

		// change the root
		(*root)->parent = new_root;
		*root = new_root;
	}
	else {
		bt_insert_non_full(*root, n);
	}
}


bool bt_is_full(bt_node *x) {
	return x->length >= NODE_SIZE;
}

void bt_make_room(bt_node *node, int i) {
	if (node->length - i > 0) {
		memmove(&node->records[i + 1], &node->records[i], (node->length - i) * sizeof(record));

		// we only move the pointers as well if the node is not a leaf
		if (!node->leaf)
			memmove(&node->children[i + 1], &node->children[i], (node->length - i) * sizeof(bt_node *));
	}
}


void bt_split_child(bt_node *parent, bt_node *node, size_t i) {
	size_t median_i = node->length / 2;
	record median = node->records[median_i];
	bt_node *new = malloc(sizeof(bt_node));
	new->leaf = node->leaf;

    // in a B+ tree we also put the median in the right child
	int old_length = node->length;
	new->length = old_length - median_i;
	assert(new->length > 0);

	// update the left node
	node->length = median_i;
	assert(old_length == node->length + new->length);

	// then we copy records from the left child to the right childt
	// new is right. node is left
	// new contains median
	memcpy(&new->records[0], &node->records[median_i], new->length * sizeof(record));
	if (!node->leaf)
	    memcpy(&new->children[0], &node->children[median_i], new->length * sizeof(bt_node *));

    assert_increasing(node);
    assert_increasing(new);

	new->next = node->next;
	new->parent = node->parent;
	node->next = new;

    // update the parent
	bt_make_room(parent, i);
	parent->children[i] = new;
	parent->records[i] = new->records[0];
	parent->length++;
}

size_t bt_search_left(bt_node *node, data d) {
	int i;
	for (i = 0; i < node->length; i++) {
		if (node->records[i].val >= d)
			break;
	}

    return i;
}

// returns the leaf where n is/should be
bt_node *bt_search_node(bt_node *root, data d) {
	if (root->leaf)
		return root;
	int i = bt_search_left(root, d);
	if (i > 0)
	    i--;

	return bt_search_node(root->children[i], d);
}

int bt_size(bt_node *root) {
    vector *v = select_one_btree(root, MIN_DATA, MAX_DATA);
    int ret = v->length;
    destroy_vector(v);
    return ret;
}


vector *select_one_btree(bt_node *root, data low, data high) {
    bt_node *ll = bt_search_node(root, low);
    bt_node *hl = bt_search_node(root, high);

    vector *ret = create_vector(0);
    bt_node *cur = ll;
    int not_counted = 0;
    int size_big = 0;
    while (cur != NULL) {
        size_big += cur->length;
        assert(cur->leaf);
        for (int i = 0; i < cur->length; i++) {
            // if we passed high, we are done
            //
            if (cur->records[i].val >= high) {

                printf("size: %d. Not counted: %d.\n", size_big, not_counted);
                return ret;
            }

            if (cur->records[i].val >= low)
                vector_insert(cur->records[i].pos, ret);
            else
                not_counted++;
        }
        if (cur == hl)
            break;
        cur = cur->next;
    }
    return ret;
}
