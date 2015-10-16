#include <string.h>
#include <assert.h>

#include "include/cs165_api.h"
#include "include/utils.h"


status OK_STATUS = {OK, NULL};
status NULL_PTR = {ERROR, "You are trying to dereference a null pointer"};

/*
 * Frees the given db operators thoroughly.
 */
void free_db_operator(db_operator *dbo) {
    if (dbo == NULL)
        return;

    free(dbo);
}

status create_db(const char* db_name, db** db) {
    if (db == NULL)
        return NULL_PTR;

    if (*db == NULL)
        *db = malloc(sizeof(db));

    (*db)->name = strdup(db_name);
    (*db)->table_count = 0;
    (*db)->max_table_count = 0;
    (*db)->tables = NULL;
    return OK_STATUS;
}


status create_table(db* db, const char* name, size_t num_columns) {
    if (db == NULL)
        return NULL_PTR;

    table *table = get_next_allocated_element(&db->table_count, &db->max_table_count,
                                                sizeof(table), &(db->tables));

    table->name = strdup(name);

    // create space for the columns
    table->cols = malloc(num_columns * sizeof(column));
    table->max_col_count = num_columns;
    table->col_count = 0;
    return OK_STATUS;
}



status create_column(table *table, const char* name, IndexType type) {
    if (table == NULL)
        return NULL_PTR;

    // grabs column from table
    column *col = get_next_allocated_element(&table->col_count, &table->max_col_count,
                                                sizeof(column), &(table->cols));

    // creates column
    col->name = strdup(name);
    col->data = NULL;
    col->count = 0;
    col->max_count = 0;
    col->index.type = type;
    col->index.index = NULL;

    return OK_STATUS;
}

status drop_db(db* db) {
    if (db == NULL)
        return NULL_PTR;

    assert(db->name != NULL);
    free(db->name);

    // destroy all tables
    if (db->tables != NULL) {
        assert(db->max_table_count > 0);
        size_t n = db->table_count;
        for (size_t i = 0; i < n; i++)
            drop_table(db, &(db->tables[i]));
        free(db->tables);
    }
    return OK_STATUS;
}

status drop_table(db *db, table* tbl) {
    if (tbl == NULL)
        return NULL_PTR;

    // table must have a name
    assert(tbl->name != NULL);
    free(tbl->name);

    // destroy all columns
    if (tbl->cols != NULL) {
        assert(tbl->max_col_count > 0);
        size_t n = tbl->col_count;
        for (size_t i = 0; i < n; i++)
            drop_column(tbl, &(tbl->cols[i]));
        assert(tbl->col_count == 0);

        // make sure the columns are freed for good measure
        if (tbl->cols != NULL) {
            free(tbl->cols);
            tbl->max_col_count = 0;
        }
    }

    // db bookkeeping
    db->table_count--;
    delete_allocated_element(tbl, db->tables);
    return OK_STATUS;
}

status drop_column(table *tbl, column* col) {
    if (col == NULL)
        return NULL_PTR;

    // column must have a name
    assert(col->name != NULL);
    free(col->name);

    if (col->index.index != NULL)
        free(col->index.index);

    if (col->data != NULL)
        free(col->data);

    tbl->col_count--;
    delete_allocated_element(col, tbl->cols);
    return OK_STATUS;
}


status relational_insert(table *tbl, list *values) {
    assert(tbl->col_count == values->length);
    assert(tbl != NULL);
    assert(values != NULL);

    // Insert each respective value into a row
    int last_count = 0;
    for (size_t i = 0; i < tbl->col_count; i++) {
        int val = pop_front(values);
        insert(tbl->cols[i], val);

        // check that all counts end up being the same for integrity
        assert(last_count == 0 || tbl->cols[i]->count != last_count);
        last_count = tbl->cols[i]->count;
    }
    assert(values->length == 0);
}

status insert(column *col, int data) {

    int *pos = get_next_allocated_element(&col->count, &col->max_count, sizeof(int), col->data);
    *pos = data;
    return OK_STATUS;
}

vector *select_one(column *col, MaybeInt low, MaybeInt high) {
    // right now we create the vector with no name
    vector *ret = create_vector(NULL);

    // performing the select
    for (size_t i = 0; i < col->count; i++) {
        if (low.present && high.present && col->data[i] < high.val
                                       && col->data[i] >= low.val)
            vector_insert(i, ret);
        else if (low.present && !high.present && col->data[i] >= low.val)
            vector_insert(i, ret);
        else if (!low.present && high.present && col->data[i] < high)
            vector_insert(i, ret);
        else if (!low.present && !high.present)
            vector_insert(i, ret);
    }
    return ret;
}

