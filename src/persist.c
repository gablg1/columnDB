#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "cs165_api.h"
#include "utils.h"
#include "sorted.h"
#include "btree.h"

#define PERSIST_DIR "persist"

/*
 * Opens file withint PERSIST_DIR
 */
FILE *persist_fopen(const char *restrict filename, const char *restrict mode) {
    char buf[NAME_SIZE];
    snprintf(buf, NAME_SIZE, "%s/%s", PERSIST_DIR, filename);
    FILE *fp = fopen(buf, mode);
    return fp;
}

void load_sorted_index(column *col, FILE *fp) {
    col->index.type = SORTED;
    col->index.index = malloc(sizeof(sorted_index));
    sorted_index *index = col->index.index;

    assert(index != NULL);
    fread(index, sizeof(sorted_index), 1, fp);

    index->positions = malloc(sizeof(size_t) * index->max_length);
    assert(index->positions != NULL);
    fread(index->positions, sizeof(size_t), index->length, fp);

    index->data = malloc(sizeof(int) * index->max_length);
    assert(index->data != NULL);
    fread(index->data, sizeof(int), index->length, fp);
}

void load_btree_index(column *col, FILE *fp) {
    // TODO: implement me
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
    col->vector = malloc(sizeof(vector));
    assert(col->vector != NULL);
    fread(col->vector, sizeof(vector), 1, fp);

    int *data = malloc(sizeof(int) * col->vector->max_length);
    assert(data != NULL);
    fread(data, sizeof(int), col->vector->length, fp);
    col->vector->buf = data;

    switch (col->index.type) {
        case (UNSORTED):
            break;
        case (PRIMARY):
            break;
        case (BTREE):
            load_btree_index(col, fp);
            break;
        case (SORTED):
            load_sorted_index(col, fp);
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
    // TODO: implement me
}

void persist_sorted_index(sorted_index *index, FILE *fp) {
    fwrite(index, sizeof(sorted_index), 1, fp);
    fwrite(index->positions, sizeof(size_t), index->length, fp);
    fwrite(index->data, sizeof(int), index->length, fp);
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
    fwrite(col->vector, sizeof(vector), 1, fp);
    fwrite(col->vector->buf, sizeof(int), col->vector->length, fp);

    switch (col->index.type) {
        case (UNSORTED):
            break;
        case (PRIMARY):
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
