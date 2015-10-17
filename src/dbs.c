#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "cs165_api.h"
#include "dbs.h"
#include "utils.h"
#include "message.h"

// structure that keeps track of all of the dbs we have in memory
db *dbs = NULL;
size_t num_of_dbs = 0;
size_t allocated_dbs = 0;

db *get_next_allocated_db(void) {
    return get_next_allocated_element(&num_of_dbs, &allocated_dbs, sizeof(db), &dbs);
}

db *get_db_by_name(const char *lookup_name) {
    for (size_t i = 0; i < num_of_dbs; i++)
        if (strcmp(dbs[i].name, lookup_name) == 0)
            return &dbs[i];
    return NULL;
}

table *get_table_by_name(const char *db_name, const char *tbl_name) {
    db *db = get_db_by_name(db_name);
    if (db == NULL)
        return NULL;

    table *tables = db->tables;
    for ( size_t i = 0; i < db->table_count; i++)
        if (strcmp(tables[i].name, tbl_name) == 0)
            return &tables[i];

    return NULL;
}

column *get_column_by_name(const char *db_name, const char *tbl_name, const char *col_name) {
    table *tbl = get_table_by_name(db_name, tbl_name);
    if (tbl == NULL)
        return NULL;

    column *cols = tbl->cols;
    for (size_t i = 0; i < tbl->col_count; i++)
        if (strcmp(cols[i].name, col_name) == 0)
            return &cols[i];

    return NULL;
}

void drop_dbs(void) {
    for (size_t i = 0; i < num_of_dbs; i++)
        drop_db(&dbs[i]);

    if (dbs != NULL)
        free(dbs);
}
