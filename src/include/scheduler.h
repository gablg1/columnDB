#include <pthread.h>

typedef struct sc_node {
    char *result_name;
    column *col;
    MaybeInt l;
    MaybeInt h;
    int length;
    struct sc_node *next;
    pthread_t thread;
    int start;
    int end;
    vector *result;
} sc_node;


void unschedule_all(void);

void schedule_select(char *var_name, column *col, MaybeInt l, MaybeInt h);

void execute_scheduled(void);

void execute_scheduled_sequentially(void);
