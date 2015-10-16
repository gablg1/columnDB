#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "cs165_api.h"
#include "dbs.h"
#include "utils.h"
#include "message.h"
#include "agnostic_vector.h"

// We use an AgnosticVector of (int) vectors to store variables
// such as var1 in
// <var1>=select(...)
AgnosticVector *vars = NULL;

void add_var(vector *v) {
    // creates the AgnosticVector if it doesn't exist yet
    if (vars == NULL)
        vars = create_agnostic_vector(sizeof(vector));

    // inserts a vector into the global AgnosticVector
    agnostic_vector_insert(v, vars);
}

vector *get_var_by_name(const char *lookup_name, message *send_msg) {
    // we know that vars->buf is actually pointing to an array of vectors
    vector **vs = vars->buf;
    for (size_t i = 0; i < vars->length; i++)
        if (strcmp(vs[i]->name, lookup_name) == 0)
            return vs[i];

    add_payload(send_msg, "Could not find db %s", lookup_name);
    return NULL;
}

void destroy_vars(void) {
    if (vars == NULL)
        return;

    vector **vs = vars->buf;
    for (size_t i = 0; i < vars->length; i++)
        free(vs[i]);

    destroy_agnostic_vector(vars);
}
