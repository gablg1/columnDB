#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

list *create_list(void) {
    list *l = malloc(sizeof(list));
    l->first = NULL;
    l->length = 0;
    return l;
}

void push_front(int val, list *root) {
    assert(root != NULL);
    node *n = malloc(sizeof(node));

    // n points to the current first element
    n->val = val;
    n->next = root->first;

    // root now points to n
    root->length++;
    root->first = n;
}

int pop_front(list *root) {
    assert(root != NULL);
    assert(root->first != NULL);

    // grab the front value
    node *front = root->first;
    int ret = front->val;

    // root now points to the second element, and we free the first
    root->first = front->next;
    free(front);
    root->length--;
    return ret;
}

void destroy_helper(node *n) {
    if (n == NULL)
        return;
    destroy_helper(n->next);
    free(n);
}

void destroy_list(list *root) {
    assert(root != NULL);

    destroy_helper(root->first);
    free(root);
}
