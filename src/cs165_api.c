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
    col->vector = create_vector();
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

    if (col->vector != NULL)
        destroy_vector(col->vector);

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
        assert(last_count == 0 || tbl->cols[i].vector->length == last_count);
        last_count = tbl->cols[i].vector->length;
    }
    assert(values->length == 0);
    return OK_STATUS;
}

status insert(column *col, int data) {
    vector_insert(data, col->vector);
    return OK_STATUS;
}

status insert_vector(column *col, vector *v) {
    // concatenate both vectors into col->vector
    vector_cat(v, col->vector);

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
        vector_insert(col->vector->buf[pos], ret);
    }
    return ret;
}

vector *add(vector *v1, vector *v2) {
    assert(v1->length == v2->length);
    vector *ret = create_vector();
    vector_cat(v1, ret);
    assert(v1->length == ret->length);
    for (size_t i = 0; i < v2->length; i++)
        ret->buf[i] += v2->buf[i];
    return ret;

}

vector *sub(vector *v1, vector *v2) {
    assert(v1->length == v2->length);
    vector *ret = create_vector();
    vector_cat(v1, ret);
    assert(v1->length == ret->length);
    for (size_t i = 0; i < v2->length; i++)
        ret->buf[i] -= v2->buf[i];
    return ret;

}

double avg(vector *values) {
    long long sum = 0;
    for (size_t i = 0; i < values->length; i++) {
        sum += values->buf[i];
    }
    return (double) sum / values->length;
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

int max(vector *values) {
    assert(values->length >= 1);
    int max = values->buf[0];
    for (size_t i = 1; i < values->length; i++) {
        if (max < values->buf[i])
            max = values->buf[i];
    }
    return max;
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
    add_payload(msg, buf);
    printf("%d\n", n);
    return OK_STATUS;
}

status tuple_long(long long n, message *msg) {
    char buf[MAX_MSG_SIZE];
    int ret = snprintf(buf, MAX_MSG_SIZE, "%lld\n", n);
    if (ret >= MAX_MSG_SIZE) {
        add_payload(msg, "Not enough space in buffer");
        return BUF_ERR;
    }
    add_payload(msg, buf);
    printf("%lld\n", n);
    return OK_STATUS;
}

status tuple_float(double f, message *msg) {
    char buf[MAX_MSG_SIZE];
    int ret = snprintf(buf, MAX_MSG_SIZE, "%.12f\n", f);
    if (ret >= MAX_MSG_SIZE) {
        add_payload(msg, "Not enough space in buffer");
        return BUF_ERR;
    }
    add_payload(msg, buf);
    printf("%.12f\n", f);
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
        case LONG_N:
            return tuple_long(var->l, msg);
            break;
        case DOUBLE_N:
            return tuple_float(var->f, msg);
            break;
    }
    return OK_STATUS;
}

vector *select_one(column *col, MaybeInt low, MaybeInt high) {
    // right now we create the vector with no name
    vector *ret = create_vector();
    vector *data = col->vector;
    // performing the select
    for (size_t i = 0; i < data->length; i++) {
        if (low.present && high.present && data->buf[i] < high.val
                                       && data->buf[i] >= low.val)
            vector_insert(i, ret);
        else if (low.present && !high.present && data->buf[i] >= low.val)
            vector_insert(i, ret);
        else if (!low.present && high.present && data->buf[i] < high.val)
            vector_insert(i, ret);
        else if (!low.present && !high.present)
            vector_insert(i, ret);
    }
    return ret;
}

