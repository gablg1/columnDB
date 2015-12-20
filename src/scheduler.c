#include <pthread.h>
#include <string.h>

#include "cs165_api.h"
#include "utils.h"
#include "dbs.h"
#include "variables.h"
#include "agnostic_vector.h"
#include "scheduler.h"


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
    root = NULL;
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

    // this has to be set on execution
    new->start = -1;
    new->end = -1;

    // We leave the return vector initialization to select
    new->result = NULL;
}

void *thr_func(void *arg) {
    sc_node *node = arg;
    assert(node->col != NULL);
    select_one_block(node->col, node->l, node->h, node->start, node->end, &(node->result));
    printf("Done with thread\n");
    pthread_exit(NULL);
}

void execute_scheduled_sequentially(void) {
    // no threads to be schedules
    if (root == NULL)
        return;

    // Currently assumes everyone in the LL is on the same column

    // Schedule each thread on some pages
    for (sc_node *cur = root; cur != NULL; cur = cur->next) {
        assert(cur->col != NULL);

        vector *v = select_one(cur->col, cur->l, cur->h);
        add_vector_var(v, cur->result_name);
    }


    printf("About to unschedule selects\n");
    unschedule_all();
}

void execute_scheduled(void) {
    // no threads to be schedules
    if (root == NULL)
        return;

    // Currently assumes everyone in the LL is on the same column
    column *col = root->col;

    // Schedule each thread on some pages
    int n_pages = 32;
    printf("Vector size: %d\n", (int) col->vector->length);
    for (size_t i = 0; i < col->vector->length; i += n_pages * PAGESIZE) {
        int start = i;
        int end = i + n_pages * PAGESIZE;
        int j = 0;
        for (sc_node *cur = root; cur != NULL; cur = cur->next) {
            assert(cur->col != NULL);
            cur->start = start;
            cur->end = end;

            printf("Creating thread %d\n", j++);
            pthread_create(&cur->thread, NULL, thr_func, cur);
        }

        printf("About to wait for the threads\n");

        // Blocks until we get all results
        for (sc_node *cur = root; cur != NULL; cur = cur->next) {
            pthread_join(cur->thread, NULL);
        }
        printf("Done with block %d\n", (int) i);
    }

    for (sc_node *cur = root; cur != NULL; cur = cur->next) {
        add_vector_var(cur->result, cur->result_name);
    }

    printf("About to unschedule selects\n");
    unschedule_all();
}
