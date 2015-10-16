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

db *get_db_by_name(const char *lookup_name, message *send_msg) {
    for (size_t i = 0; i < num_of_dbs; i++)
        if (strcmp(dbs[i].name, lookup_name) == 0)
            return &dbs[i];

    add_payload(send_msg, "Could not find db %s", lookup_name);
    return NULL;
}

table *get_table_by_name(const char *db_name, const char *tbl_name, message *send_msg) {
    db *db = get_db_by_name(db_name, send_msg);
    if (db == NULL)
        return NULL;

    table *tables = db->tables;
    for ( size_t i = 0; i < db->table_count; i++)
        if (strcmp(tables[i].name, tbl_name) == 0)
            return &tables[i];

    add_payload(send_msg, "Could not find table %s in db %s", tbl_name, db_name);
    return NULL;
}

column *get_column_by_name(const char *db_name, const char *tbl_name, const char *col_name, message *send_msg) {
    table *tbl = get_table_by_name(db_name, tbl_name, send_msg);
    if (tbl == NULL)
        return NULL;

    column *cols = tbl->cols;
    for (size_t i = 0; i < tbl->col_count; i++)
        if (strcmp(cols[i].name, col_name) == 0)
            return &cols[i];

    add_payload(send_msg, "Could not find column %s in table %s", col_name, tbl_name);
    return NULL;
}

void drop_dbs(void) {
    for (size_t i = 0; i < num_of_dbs; i++)
        drop_db(&dbs[i]);

    if (dbs != NULL)
        free(dbs);
}
