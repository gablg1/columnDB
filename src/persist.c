#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "cs165_api.h"
#include "utils.h"
#include "sorted.h"
#include "btree.h"

#define PERSIST_DIR "data"

/*
 * Opens file withdata PERSIST_DIR
 */
FILE *persist_fopen(const char *restrict filename, const char *restrict mode) {
    char buf[NAME_SIZE];
    snprintf(buf, NAME_SIZE, "%s/%s", PERSIST_DIR, filename);

    FILE *fp = fopen(buf, mode);
    return fp;
}

vector *load_vector(FILE *fp) {
    vector *v = malloc(sizeof(vector));
    assert(v != NULL);
    fread(v, sizeof(vector), 1, fp);

    data *d = malloc(sizeof(data) * v->max_length);
    assert(d != NULL);
    fread(d, sizeof(data), v->length, fp);
    v->buf = d;
    return v;
}

column_index load_sorted_index(FILE *fp) {
    column_index i;
    i.type = SORTED;
    i.index = malloc(sizeof(sorted_index));
    sorted_index *index = i.index;

    assert(index != NULL);
    //fread(index, sizeof(sorted_index), 1, fp); // no need anymore

    index->positions = load_vector(fp);
    index->data = load_vector(fp);
    return i;
}


// bro_last is the last child of the brother node to my immediate left
// my_last is where this function will place this node's last child
// These are both used to correctly load bt_node->next
bt_node *load_btree_node(FILE *fp) {
    bt_node *ret = malloc(sizeof(bt_node));
    assert(ret != NULL);

    fread(ret, sizeof(bt_node), 1, fp);

    if (!ret->leaf) {
        for (int i = 0; i < ret->length; i++) {
            ret->children[i] = load_btree_node(fp);
        }
    }

    return ret;
}

void place_btree_nexts(bt_node *root) {
    if (!root->leaf) {
        for (int i = 0; i < root->length; i++)
            root->children[i]->next = root->children[i + 1];

        if (root->next != NULL)
            root->children[root->length - 1]->next = root->next->children[0];
        else
            root->children[root->length - 1]->next = NULL;

        for (int i = 0; i < root->length; i++)
            place_btree_nexts(root->children[i]);
    }

}

column_index load_btree_index(FILE *fp) {
    column_index i;
    i.type = BTREE;
    i.index = load_btree_node(fp);
    place_btree_nexts(i.index);
    return i;
}

/*
 * Loads a column from file at filename
 * Column is put in col
 **/
void load_column(const char *filename, column *col) {
    assert(col != NULL);

    // read the column struct and data from file
    FILE *fp = persist_fopen(filename, "r");
    assert(fp != NULL);

    fread(col, sizeof(column), 1, fp);
    col->vector = load_vector(fp);

    switch (col->type) {
        case (UNSORTED):
            break;
        case (PRIMARY):
            break;
    }
    switch (col->index.type) {
        case (NO_INDEX):
            break;
        case (BTREE):
            col->index = load_btree_index(fp);
            break;
        case (SORTED):
            col->index = load_sorted_index(fp);
            break;
    }

    fclose(fp);
}

void load_table(const char* filename, table *tbl) {
    assert(tbl != NULL);

    FILE *fp = persist_fopen(filename, "r");
    assert(fp != NULL);

    // first we get the raw table data
    fread(tbl, sizeof(table), 1, fp);

    // now we allocate space for the columns to come
    assert(tbl->max_col_count > 0);
    tbl->cols = malloc(sizeof(column) * tbl->max_col_count);
    assert(tbl->cols != NULL);

    // now what follows is a series of column names followed by newlines
    char buf[NAME_SIZE];
    for (size_t i = 0; i < tbl->col_count; i++) {
        // reads the filename of a column, stripping the newline
        fgets(buf, NAME_SIZE, fp);
        newline_to_null(buf);

        load_column(buf, &(tbl->cols[i]));
    }
    fclose(fp);

    if (tbl->primary > -1)
        assert(tbl->cols[tbl->primary].type == PRIMARY);
}

void load_db(const char* filename, db *dbp) {
    assert(dbp != NULL);

    FILE *fp = persist_fopen(filename, "r");
    assert(fp != NULL);

    // first we get the raw db data
    fread(dbp, sizeof(db), 1, fp);

    // now we allocate space for the tables to come
    assert(dbp->max_table_count > 0);
    dbp->tables = malloc(sizeof(table) * dbp->max_table_count);
    assert(dbp->tables != NULL);

    // now what follows is a series of table names followed by newlines
    char buf[NAME_SIZE];
    for (size_t i = 0; i < dbp->table_count; i++) {
        // reads the filename of a table, stripping the newline
        fgets(buf, NAME_SIZE, fp);
        newline_to_null(buf);

        load_table(buf, &(dbp->tables[i]));
    }
    fclose(fp);
}


void persist_btree_index(bt_node *root, FILE *fp) {
    assert(root != NULL);
    fwrite(root, sizeof(bt_node), 1, fp);
    if (!root->leaf) {
        for (int i = 0; i < root->length; i++)
            persist_btree_index(root->children[i], fp);
    }
}

void persist_vector(vector *v, FILE *fp) {
    fwrite(v, sizeof(vector), 1, fp);
    fwrite(v->buf, sizeof(data), v->length, fp);
}

void persist_sorted_index(sorted_index *index, FILE *fp) {
    // fwrite(index, sizeof(sorted_index), 1, fp); // not needed anymore
    persist_vector(index->positions, fp);
    persist_vector(index->data, fp);
}


void persist_column(db *db, table *tbl, column *col) {
    assert(db != NULL && tbl != NULL && col != NULL);
    char filename[NAME_SIZE];
    int ret = snprintf(filename, NAME_SIZE, "%s/%s.%s.%s", PERSIST_DIR, db->name, tbl->name, col->name);
    assert(ret < NAME_SIZE);

    FILE *fp = fopen(filename, "w");
    assert(fp != NULL);

    // here we persist both the column struct and the data
    fwrite(col, sizeof(column), 1, fp);
    persist_vector(col->vector, fp);

    switch (col->type) {
        case (UNSORTED):
            break;
        case (PRIMARY):
            break;
    }
    switch (col->index.type) {
        case (NO_INDEX):
            break;
        case (BTREE):
            persist_btree_index(col->index.index, fp);
            break;
        case (SORTED):
            persist_sorted_index(col->index.index, fp);
            break;
    }
    fclose(fp);
}

void persist_table(db *db, table *tbl) {
    assert(db != NULL && tbl != NULL);

    char filename[NAME_SIZE];
    int ret = snprintf(filename, NAME_SIZE, "%s/%s.%s", PERSIST_DIR, db->name, tbl->name);
    assert(ret < NAME_SIZE);

    FILE *fp = fopen(filename, "w");
    assert(fp != NULL);
    fwrite(tbl, sizeof(table), 1, fp);

    // now what follows is a series of column names followed by newlines
    for (size_t i = 0; i < tbl->col_count; i++) {
        // reads the filename of a table, stripping the newline
        int ret = snprintf(filename, NAME_SIZE, "%s.%s.%s\n", db->name, tbl->name, tbl->cols[i].name);
        assert(ret < NAME_SIZE);
        fputs(filename, fp);

        persist_column(db, tbl, &(tbl->cols[i]));
    }

    fclose(fp);
}

void persist_db(db *dbp) {
    assert(dbp != NULL);

    char filename[NAME_SIZE];
    int ret = snprintf(filename, NAME_SIZE, "%s/%s", PERSIST_DIR, dbp->name);
    assert(ret < NAME_SIZE);

    FILE *fp = fopen(filename, "w");
    assert(fp != NULL);
    fwrite(dbp, sizeof(db), 1, fp);

    // now what follows is a series of table names followed by newlines
    for (size_t i = 0; i < dbp->table_count; i++) {
        // reads the filename of a table, stripping the newline
        int ret = snprintf(filename, NAME_SIZE, "%s.%s\n", dbp->name, dbp->tables[i].name);
        assert(ret < NAME_SIZE);
        fputs(filename, fp);

        persist_table(dbp, &(dbp->tables[i]));
    }
    fclose(fp);

}
