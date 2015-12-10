#include <pthread.h>
#include <string.h>

#include "cs165_api.h"
#include "utils.h"
#include "dbs.h"
#include "variables.h"
#include "agnostic_vector.h"

typedef struct sc_node {
    char *result_name;
    column *col;
    MaybeInt l;
    MaybeInt h;
    int length;
    struct sc_node *next;
    pthread_t thread;
} sc_node;

sc_node *root;

void destroy_sc_node(sc_node *n) {
    assert(n != NULL);
    assert(n->result_name != NULL);
    free(n->result_name);
    free(n);
}

void unschedule_helper(sc_node *n) {
    if (n == NULL)
        return;
    unschedule_helper(n->next);
    destroy_sc_node(n);
}

void unschedule_all(void) {
    unschedule_helper(root);
}

void schedule_select(char *var_name, column *col, MaybeInt l, MaybeInt h) {
    assert(col != NULL);
    sc_node *new = malloc(sizeof(sc_node));
    assert(new != NULL);
    new->col = col;
    new->l = l;
    new->h = h;
    new->next = root;
    if (root != NULL)
        new->length = root->length + 1;
    else
        new->length = 0;
    new->result_name = strdup(var_name);
    root = new;
}

void *thr_func(void *arg) {
    sc_node *node = arg;
    assert(node->col != NULL);
    vector *v = select_one(node->col, node->l, node->h);
    printf("Done with thread\n");
    pthread_exit(v);
}


void execute_scheduled(void) {
    // no threads to be schedules
    if (root == NULL)
        return;

    for (sc_node *cur = root; cur != NULL; cur = cur->next) {
        assert(cur->col != NULL);
        pthread_create(&cur->thread, NULL, thr_func, cur);
    }

    // Blocks until we get all results
    for (sc_node *cur = root; cur != NULL; cur = cur->next) {
        vector *result;
        pthread_join(cur->thread, (void **) &result);
        add_vector_var(result, cur->result_name);
    }

    printf("Got here\n");
    unschedule_all();
}
