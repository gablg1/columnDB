#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "cs165_api.h"
#include "dbs.h"
#include "utils.h"
#include "message.h"
#include "agnostic_vector.h"
#include "variables.h"


// We use an AgnosticVector of variables to store variables
// such as var1 in
// <var1>=select(...)
AgnosticVector *vars = NULL;

void add_int_var(int n, const char *name) {
    assert(name != NULL);
    // creates the AgnosticVector if it doesn't exist yet
    if (vars == NULL)
        vars = create_agnostic_vector(sizeof(variable));


    // inserts a vector into the global AgnosticVector
    variable nod;
    nod.name = strdup(name);
    nod.type = INT_N;
    nod.i = n;
    agnostic_vector_insert(&nod, vars);
}

void add_long_var(long long n, const char *name) {
    assert(name != NULL);
    // creates the AgnosticVector if it doesn't exist yet
    if (vars == NULL)
        vars = create_agnostic_vector(sizeof(variable));

    // inserts a vector into the global AgnosticVector
    variable nod;
    nod.name = strdup(name);
    nod.type = LONG_N;
    nod.l = n;
    agnostic_vector_insert(&nod, vars);
}

void add_float_var(double f, const char *name) {
    assert(name != NULL);
    // creates the AgnosticVector if it doesn't exist yet
    if (vars == NULL)
        vars = create_agnostic_vector(sizeof(variable));

    // inserts a vector into the global AgnosticVector
    variable n;
    n.name = strdup(name);
    n.type = DOUBLE_N;
    n.f = f;
    agnostic_vector_insert(&n, vars);
}

void add_vector_var(vector *v, const char *name) {
    assert(name != NULL);
    // creates the AgnosticVector if it doesn't exist yet
    if (vars == NULL)
        vars = create_agnostic_vector(sizeof(variable));

    // inserts a vector into the global AgnosticVector
    variable n;
    n.name = strdup(name);
    n.type = VECTOR_N;
    n.v = v;
    agnostic_vector_insert(&n, vars);
}

variable *get_var_by_name(const char *lookup_name) {
    if (vars == NULL)
        return NULL;
    // we know that vars->buf is actually pointing to an array of vectors
    variable *vs = vars->buf;
    for (size_t i = 0; i < vars->length; i++)
        if (strcmp(vs[i].name, lookup_name) == 0)
            return &vs[i];

    return NULL;
}

void destroy_vars(void) {
    if (vars == NULL)
        return;

    variable *vs = vars->buf;
    for (size_t i = 0; i < vars->length; i++) {
        assert(vs[i].name != NULL);
        free(vs[i].name);

        if (vs[i].type == VECTOR_N)
            destroy_vector(vs[i].v);
    }

    destroy_agnostic_vector(vars);
    vars = NULL;
}
