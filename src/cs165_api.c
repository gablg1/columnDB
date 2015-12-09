#include <string.h>
#include <sys/socket.h>

#include "cs165_api.h"
#include "utils.h"
#include "dbs.h"
#include "variables.h"
#include "agnostic_vector.h"
#include "sorted.h"
#include "btree.h"



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
                                                sizeof(struct table), &(db->tables));

    strncpy(table->name, name, NAME_SIZE);

    // create space for the columns
    table->cols = malloc(num_columns * sizeof(column));
    table->max_col_count = num_columns;
    table->col_count = 0;
    table->primary = -1;
    return OK_STATUS;
}



status create_column(table *table, const char* name, ColumnType type) {
    if (table == NULL)
        return NULL_PTR;

    // grabs column from table
    column *col = get_next_allocated_element(&table->col_count, &table->max_col_count,
                                                sizeof(column), &(table->cols));

    // creates column
    strncpy(col->name, name, NAME_SIZE);
    col->vector = create_vector(0);

    // creates column index if present
    col->type = type;
    if (type == PRIMARY)
        table->primary = table->col_count - 1;
    create_index(col, NO_INDEX);

    return OK_STATUS;
}

status create_index(column *col, IndexType type) {
    col->index.type = type;
    switch (type) {
        case (NO_INDEX):
            col->index.index = NULL;
            break;
        case (BTREE):
            col->index.index = create_btree_index(col->vector);
            break;
        case (SORTED):
            col->index.index = create_sorted_index(col->vector);
            break;
    }
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

bool sort_table_by_primary_column(table *tbl) {
    assert(tbl->primary != -1);

    vector *sorted_pos = sort_vector(tbl->cols[tbl->primary].vector);
    for (size_t i = 0; i < tbl->col_count; i++) {
        if (i != (size_t) tbl->primary)
            sort_vector_from_positions(&(tbl->cols[i].vector), sorted_pos);
    }
    return true;

}

status relational_insert(table *tbl, list *values) {
    assert(tbl->col_count == values->length);
    assert(tbl != NULL);
    assert(values != NULL);

    // Insert each respective value into a row
    size_t last_count = 0;
    for (size_t i = 0; i < tbl->col_count; i++) {
        data val = pop_front(values);
        insert(&tbl->cols[i], val);

        // check that all counts end up being the same for integrity
        assert(last_count == 0 || tbl->cols[i].vector->length == last_count);
        last_count = tbl->cols[i].vector->length;
    }

    // guarantees the table will be sorted if we have a primary index
    if (tbl->primary > -1)
        sort_table_by_primary_column(tbl);

    assert(values->length == 0);
    return OK_STATUS;
}

status insert(column *col, data data) {
    size_t pos = vector_insert(data, col->vector);

    record r;
    switch (col->index.type) {
        case (NONE):
            break;
        case (BTREE):
            r.val = data;
            r.pos = pos;
            bt_insert(col->index.index, r);
            break;
        case (SORTED):
            assert(false);
            //insert_sorted(col->index.index, data, pos);
            break;
    }

    return OK_STATUS;
}

status insert_vector(column *col, vector *v) {
    // concatenate both vectors into col->vector
    vector_cat(v, col->vector);

    return OK_STATUS;
}



status load(int client_fd) {
    off_t size;
    int length = recv(client_fd, &size, sizeof(off_t), 0);
    if (length < 1)
        exit(1);

    FILE* fp = fdopen(client_fd, "r");
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
        vector *v = create_vector(0);
        assert(v != NULL);
        agnostic_vector_insert(&v, values);

    }

    table *tbl = get_table_by_name(db_name, tbl_name);
    assert(ag_cols->length == tbl->col_count);

    while (fgets(buf, 1024, fp))
    {
        // this signals that we're done
        if (strcmp(buf, "feedface\n") == 0)
            break;

        line = buf;
        size_t count = 0;
        char *token;
        while ((token = strsep(&line, ",")) != NULL) {
            data val = atoi(token);
            vector_insert(val, vals[count++]);
        }
        assert(count == ag_cols->length);
    }

    for (size_t i = 0; i < values->length; i++) {
        vector_cat(cols[i]->vector, vals[i]);
    }

    if (tbl->primary > -1)
        sort_table_by_primary_column(tbl);

    destroy_agnostic_vector(ag_cols);
    destroy_agnostic_vector(values);
    return OK_STATUS;
}

vector *fetch(vector *values, vector *positions) {
    vector *ret = create_vector(0);

    for (size_t i = 0; i < positions->length; i++) {
        size_t pos = positions->buf[i];
        vector_insert(values->buf[pos], ret);
    }
    return ret;
}

void nested_join(vector **r1, vector **r2, vector *pos1, vector *val1, vector *pos2, vector *val2) {
    // we have no a priori knowledge of what the vector sizes should be
    *r1 = create_vector(0);
    *r2 = create_vector(0);

    // we probe the smaller vector hoping for faster join
        for (size_t i = 0; i < pos1->length; i += PAGESIZE) {
            for (size_t j = 0; j < pos2->length; j += PAGESIZE) {
                size_t lim_r = (i + PAGESIZE > pos1->length) ? pos1->length : i + PAGESIZE;
                for (size_t r = i; r < lim_r; r++) {
                    size_t lim_j = (j + PAGESIZE > pos2->length) ? pos2->length : j + PAGESIZE;
                    for (size_t m = j; m < lim_j; m++) {
                        if (val1->buf[r] == val2->buf[m]) {
                            vector_insert(pos1->buf[r], *r1);
                            vector_insert(pos2->buf[m], *r2);
                        }
                    }
                }
            }
        }


    assert((*r1)->length == (*r2)->length);
}

void hashjoin(vector **r1, vector **r2, vector *pos1, vector *val1, vector *pos2, vector *val2) {
    nested_join(r1, r2, pos1, val1, pos2, val2);
}

vector *add(vector *v1, vector *v2) {
    assert(v1->length == v2->length);
    vector *ret = create_vector(v1->length);
    vector_cat(ret, v1);
    assert(v1->length == ret->length);
    for (size_t i = 0; i < v2->length; i++)
        ret->buf[i] += v2->buf[i];
    return ret;

}

vector *sub(vector *v1, vector *v2) {
    assert(v1->length == v2->length);
    vector *ret = create_vector(v1->length);
    vector_cat(ret, v1);
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

data min(vector *values) {
    assert(values->length >= 1);
    data min = values->buf[0];
    for (size_t i = 1; i < values->length; i++) {
        if (min > values->buf[i])
            min = values->buf[i];
    }
    return min;
}

data max(vector *values) {
    assert(values->length >= 1);
    data max = values->buf[0];
    for (size_t i = 1; i < values->length; i++) {
        if (max < values->buf[i])
            max = values->buf[i];
    }
    return max;
}

status tuple_vectors(vector *val1, vector* val2, message *msg) {
    assert(val1 != NULL && val2 != NULL);
    int msg_size = MAX_MSG_SIZE;
    char *buf = malloc(msg_size * sizeof(char));
    assert(buf != NULL);
    data pos = 0;
    assert(val1->length == val2->length);
    for (size_t i = 0; i < val1->length; i++) {
        if (pos + 50 >= msg_size) {
            msg_size *= 2;
            buf = realloc(buf, msg_size * sizeof(char));
            assert(buf != NULL);
        }
        pos += snprintf(&buf[pos], msg_size - pos, "%lld,%lld\n", val1->buf[i], val2->buf[i]);

        // also prdata it on the server
        printf("%lld,%lld\n", val1->buf[i], val2->buf[i]);
    }
    //strip the last \n
    buf[pos--] = '\0';

    if (val1->length <= 0) {
        snprintf(buf, MAX_MSG_SIZE, "Empty vector");
        msg->status = OK_WAIT_FOR_RESPONSE;
    }
    add_payload(msg, buf);
    return OK_STATUS;
}
status tuple_vector(vector *values, message *msg) {
    assert(values != NULL);
    char buf[MAX_MSG_SIZE];
    data pos = 0;
    for (size_t i = 0; i < values->length; i++) {
        pos += snprintf(&buf[pos], MAX_MSG_SIZE - pos, "%lld\n", values->buf[i]);
        if (pos >= MAX_MSG_SIZE) {
            add_payload(msg, "Not enough space in buffer");
            return BUF_ERR;
        }

        // also prdata it on the server
        printf("%lld\n", values->buf[i]);
    }
    //strip the last \n
    buf[pos--] = '\0';

    if (values->length <= 0) {
        snprintf(buf, MAX_MSG_SIZE, "Empty vector");
        msg->status = OK_WAIT_FOR_RESPONSE;
    }
    add_payload(msg, buf);
    return OK_STATUS;
}

status tuple_int(data n, message *msg) {
    char buf[MAX_MSG_SIZE];
    data ret = snprintf(buf, MAX_MSG_SIZE, "%lld\n", n);
    if (ret >= MAX_MSG_SIZE) {
        add_payload(msg, "Not enough space in buffer");
        return BUF_ERR;
    }
    add_payload(msg, buf);
    printf("%lld\n", n);
    return OK_STATUS;
}

status tuple_long(long long n, message *msg) {
    char buf[MAX_MSG_SIZE];
    data ret = snprintf(buf, MAX_MSG_SIZE, "%lld\n", n);
    if (ret >= MAX_MSG_SIZE) {
        add_payload(msg, "Not enough space in buffer");
        return BUF_ERR;
    }
    add_payload(msg, buf);
    printf("%lld", n);
    return OK_STATUS;
}

status tuple_float(double f, message *msg) {
    char buf[MAX_MSG_SIZE];
    data ret = snprintf(buf, MAX_MSG_SIZE, "%.12f\n", f);
    if (ret >= MAX_MSG_SIZE) {
        add_payload(msg, "Not enough space in buffer");
        return BUF_ERR;
    }
    add_payload(msg, buf);
    printf("%.12f", f);
    return OK_STATUS;
}

status tuple(variable *var, message *msg) {
    assert(var != NULL);
    msg->status = OK_IMPORTANT;

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

vector *select_one_unsorted(vector *v, data l, data h) {
    // right now we create the vector with no name
    vector *ret = create_vector(0);

    // performing the select;
    for (size_t i = 0; i < v->length; i++) {
        if (v->buf[i] >= l && v->buf[i] < h)
            vector_insert(i, ret);
    }
    return ret;
}

vector *select_one_primary(vector *v, data l, data h) {
    // lp is the left most element >= than low
    size_t lp = binary_search_left(v->buf, l, v->length);
    // hp is the rightmost element <= than high
    size_t hp = binary_search_right(v->buf, h, v->length);

    vector *ret = create_vector(hp - lp);

    // performing the select
    for (size_t i = lp; i < hp; i++)
        vector_insert(i, ret);
    return ret;
}

vector *select_two(vector *pos_vec, vector *val_vec, MaybeInt low, MaybeInt high) {
    data l = (low.present) ? low.val : MIN_DATA;
    data h = (high.present) ? high.val : MAX_DATA;

    vector *ret = create_vector(0);

    // performing the select;
    for (size_t i = 0; i < val_vec->length; i++) {
        if (val_vec->buf[i] >= l && val_vec->buf[i] < h)
            vector_insert(pos_vec->buf[i], ret);
    }
    return ret;
}

vector *select_one(column *col, MaybeInt low, MaybeInt high) {
    data l = (low.present) ? low.val : MIN_DATA;
    data h = (high.present) ? high.val : MAX_DATA;

    switch (col->type) {
        case (PRIMARY):
            return select_one_primary(col->vector, l, h);
            break;
        case (UNSORTED):
            switch (col->index.type) {
                case (NO_INDEX):
                    return select_one_unsorted(col->vector, l, h);
                    break;
                case (BTREE):
                    return select_one_btree(col->index.index, l, h);
                    break;
                case (SORTED):
                    return select_one_sorted(col->index.index, l, h);
                    break;
            }
            break;
    }
    return NULL;
}

