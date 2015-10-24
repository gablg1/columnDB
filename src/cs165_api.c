#include <string.h>
#include <assert.h>

#include "cs165_api.h"
#include "utils.h"
#include "dbs.h"
#include "variables.h"
#include "agnostic_vector.h"


status OK_STATUS = {OK, NULL};
status NULL_PTR = {ERROR, "You are trying to dereference a null pointer"};
status FILE_ERR = {ERROR, "Unable to open file"};
status BUF_ERR = {ERROR, "Not enough space in buffer"};

/*
 * Frees the given db operators thoroughly.
 */
void free_db_operator(db_operator *dbo) {
    if (dbo == NULL)
        return;

    free(dbo);
}

status create_db(const char* db_name, db** db) {
    assert(db != NULL);

    if (*db == NULL)
        *db = malloc(sizeof(db));

    strncpy((*db)->name, db_name, NAME_SIZE);
    (*db)->table_count = 0;
    (*db)->max_table_count = 0;
    (*db)->tables = NULL;
    return OK_STATUS;
}


status create_table(db* db, const char* name, size_t num_columns) {
    assert(db != NULL);

    table *table = get_next_allocated_element(&db->table_count, &db->max_table_count,
                                                sizeof(table), &(db->tables));

    strncpy(table->name, name, NAME_SIZE);

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
    strncpy(col->name, name, NAME_SIZE);
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

    // db must have a name
    assert(db->name[0] != '\0');

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
    assert(tbl->name[0] != '\0');

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
    return OK_STATUS;
}

status drop_column(table *tbl, column* col) {
    if (col == NULL)
        return NULL_PTR;

    // column must have a name
    assert(col->name[0] != '\0');

    if (col->index.index != NULL)
        free(col->index.index);

    if (col->data != NULL)
        free(col->data);

    tbl->col_count--;
    return OK_STATUS;
}


status relational_insert(table *tbl, list *values) {
    assert(tbl->col_count == values->length);
    assert(tbl != NULL);
    assert(values != NULL);

    // Insert each respective value into a row
    size_t last_count = 0;
    for (size_t i = 0; i < tbl->col_count; i++) {
        int val = pop_front(values);
        insert(&tbl->cols[i], val);

        // check that all counts end up being the same for integrity
        assert(last_count == 0 || tbl->cols[i].count == last_count);
        last_count = tbl->cols[i].count;
    }
    assert(values->length == 0);
    return OK_STATUS;
}

status insert(column *col, int data) {

    int *pos = get_next_allocated_element(&col->count, &col->max_count, sizeof(int), &col->data);
    *pos = data;
    return OK_STATUS;
}

status insert_vector(column *col, vector *v) {
    // first we make sure the column has enough space to hold the new data
    size_t new_max_count = col->max_count;
    if (new_max_count == 0)
        new_max_count = 8;

    while (new_max_count < col->count + v->length) {
        new_max_count *= 2;
    }

    if (new_max_count > col->max_count) {
        col->data = realloc(col->data, new_max_count * sizeof(int));
        assert(col->data != NULL);
        col->max_count = new_max_count;
    }

    memcpy(col->data + col->count, v->buf, v->length * sizeof(int));
    col->count += v->length;

    return OK_STATUS;
}

status load(const char *filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
        return FILE_ERR;

    AgnosticVector *ag_cols = create_agnostic_vector(sizeof(column *));
    column **cols = ag_cols->buf;
    AgnosticVector *values = create_agnostic_vector(sizeof(vector *));
    vector **vals = values->buf;

    // buffer for storing lines
    char buf[1024];
    char *line = buf;
    assert(line != NULL);

    // first we grab the first line and strip the "\n"
    fgets(line, 1024, fp);
    line = strsep(&line, "\n");

    char *tmp, *db_name, *tbl_name, *col_name;
    while ((tmp = strsep(&line, ".")) != NULL) {
        db_name = tmp;
        tbl_name = strsep(&line, ".");
        col_name = strsep(&line, ",");

        // this can be optimized by only looking up the table once
        // since all tables must be the same
        column *col = get_column_by_name(db_name, tbl_name, col_name);
        assert(col != NULL);
        agnostic_vector_insert(&col, ag_cols);

        // each v will hold the values to be inserted in each column
        vector *v = create_vector();
        assert(v != NULL);
        agnostic_vector_insert(&v, values);

    }

    table *tbl = get_table_by_name(db_name, tbl_name);
    assert(ag_cols->length == tbl->col_count);

    while (fgets(buf, 1024, fp))
    {
        line = buf;
        size_t count = 0;
        char *token;
        while ((token = strsep(&line, ",")) != NULL) {
            int val = atoi(token);
            vector_insert(val, vals[count++]);
        }
        assert(count == ag_cols->length);
    }

    for (size_t i = 0; i < values->length; i++) {
        insert_vector(cols[i], vals[i]);
    }

    fclose(fp);
    destroy_agnostic_vector(ag_cols);
    return OK_STATUS;
}

vector *fetch(column *col, vector *positions) {
    vector *ret = create_vector();

    for (size_t i = 0; i < positions->length; i++) {
        size_t pos = positions->buf[i];
        vector_insert(col->data[pos], ret);
    }
    return ret;
}

int min(vector *values) {
    assert(values->length >= 1);
    int min = values->buf[0];
    for (size_t i = 1; i < values->length; i++) {
        if (min > values->buf[i])
            min = values->buf[i];
    }
    return min;
}

status tuple_vector(vector *values, message *msg) {
    assert(values != NULL);
    char buf[MAX_MSG_SIZE];
    int pos = 0;
    for (size_t i = 0; i < values->length; i++) {
        pos += snprintf(&buf[pos], MAX_MSG_SIZE - pos, "%d\n", values->buf[i]);
        if (pos >= MAX_MSG_SIZE) {
            add_payload(msg, "Not enough space in buffer");
            return BUF_ERR;
        }

        // also print it on the server
        printf("%d\n", values->buf[i]);
    }
    add_payload(msg, buf);
    return OK_STATUS;
}

status tuple_int(int n, message *msg) {
    char buf[MAX_MSG_SIZE];
    int ret = snprintf(buf, MAX_MSG_SIZE, "%d\n", n);
    if (ret >= MAX_MSG_SIZE) {
        add_payload(msg, "Not enough space in buffer");
        return BUF_ERR;
    }
    return OK_STATUS;
}

status tuple_float(float f, message *msg) {
    char buf[MAX_MSG_SIZE];
    int ret = snprintf(buf, MAX_MSG_SIZE, "%f\n", f);
    if (ret >= MAX_MSG_SIZE) {
        add_payload(msg, "Not enough space in buffer");
        return BUF_ERR;
    }
    return OK_STATUS;
}

status tuple(variable *var, message *msg) {
    assert(var != NULL);

    switch (var->type) {
        case VECTOR_N:
            return tuple_vector(var->v, msg);
            break;
        case INT_N:
            return tuple_int(var->i, msg);
            break;
        case FLOAT_N:
            return tuple_float(var->f, msg);
            break;
    }
}

vector *select_one(column *col, MaybeInt low, MaybeInt high) {
    // right now we create the vector with no name
    vector *ret = create_vector();

    // performing the select
    for (size_t i = 0; i < col->count; i++) {
        if (low.present && high.present && col->data[i] < high.val
                                       && col->data[i] >= low.val)
            vector_insert(i, ret);
        else if (low.present && !high.present && col->data[i] >= low.val)
            vector_insert(i, ret);
        else if (!low.present && high.present && col->data[i] < high.val)
            vector_insert(i, ret);
        else if (!low.present && !high.present)
            vector_insert(i, ret);
    }
    return ret;
}

