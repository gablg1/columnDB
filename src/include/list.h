#ifndef LL_H
#define LL_H

/*
 *
 * Here we implement a basic linked list for integers
 */
typedef struct node {
    struct node *next;
    int val;
} node;

typedef struct list {
    // length is only used in the root
    size_t length;
    node *first;
} list;


list *create_list(void);

void destroy_list(list *root);

void push_front(int val, list *root);

int pop_front(list *root);

#endif /* LL_H */
