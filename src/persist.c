#include <stdio.h>
#include <assert.h>

#include "cs165_api.h"

#define PERSIST_DIR "persist"
#define MASTER_DBS_FILENAME "dbs"

/*
 * Opens file withint PERSIST_DIR
 */
FILE *persist_fopen(const char *restrict filename, const char *restrict mode) {
    char buf[NAME_SIZE];
    snprintf(buf, NAME_SIZE, "%s/%s", PERSIST_DIR, filename);
    FILE *fp = fopen(buf, "r");
    assert(fp != NULL);
    return fp;
}

/*
 * Loads a column from file at filename
 * Column is put in col
 **/
void load_column(const char *filename, column *col) {
    assert(col != NULL);

    // read the column struct and data from file
    FILE *fp = persist_fopen(filename);

    fread(col, sizeof(col), 1, fp);
    int *data = malloc(sizeof(int) * col->max_count);
    assert(data != NULL);
    fread(data, sizeof(int), col->count, fp);
    fclose(fp);

    // column points to the data
    col->data = data;

}

void load_table(const char* filename, table *tbl) {
    assert(tbl != NULL);

    FILE *fp = persist_fopen(filename, "r");

    // first we get the raw table data
    fread(tbl, sizeof(table), 1, fp);

    // now we allocate space for the columns to come
    tbl->cols = malloc(sizeof(column) * tbl->max_col_count);
    assert(tbl->cols != NULL);

    // now what follows is a series of column names followed by newlines
    char buf[NAME_SIZE];
    for (size_t i = 0; i < tbl->col_count; i++) {
        // reads the filename of a column, stripping the newline
        fgets(buf, NAME_SIZE, fp);
        int n = strlen(buf);
        buf[n] = '\0';

        load_column(buf, &(tbl->cols[i]));
    }
    fclose(fp);
}

void load_db(const char* filename, db *db) {
    assert(db != NULL);

    FILE *fp = persist_fopen(filename, "r");

    // first we get the raw db data
    fread(db, sizeof(db), 1, fp);

    // now we allocate space for the tables to come
    db->tables = malloc(sizeof(table) * db->max_table_count);
    assert(db->tables != NULL);

    // now what follows is a series of table names followed by newlines
    char buf[NAME_SIZE];
    for (size_t i = 0; i < db->table_count; i++) {
        // reads the filename of a table, stripping the newline
        fgets(buf, NAME_SIZE, fp);
        int n = strlen(buf);
        buf[n] = '\0';

        load_table(buf, &(db->tables[i]));
    }
    fclose(fp);
}


void persist_column(db *db, table *tbl, column *col) {
    assert(db != NULL && tbl != NULL && col != NULL);
    char filename[NAME_SIZE];
    int ret = snprintf(filename, NAME_SIZE, "%s/%s.%s.%s", PERSIST_DIR, db->name, tbl->name, col->name);
    assert(ret < NAME_SIZE);

    FILE *fp = fopen(filename, "w");

    // here we persist both the column struct and the data
    fwrite(col, sizeof(column), 1, fp);
    fwrite(col->data, sizeof(int), col->count, fp);
    fclose(fp);
}

void persist_table(db *db, table *tbl) {
    assert(db != NULL && tbl != NULL);
    for (size_t i = 0; i < tbl->col_count; i++)
        persist_column(db, tbl, &(tbl->cols[i]));

    char filename[NAME_SIZE];
    int ret = snprintf(filename, NAME_SIZE, "%s/%s.%s", PERSIST_DIR, db->name, tbl->name);
    assert(ret < NAME_SIZE);

    FILE *fp = fopen(filename, "w");
    fwrite(tbl, sizeof(table), 1, fp);
    fclose(fp);
}

void persist_db(db *db) {
    assert(db != NULL);
    for (size_t i = 0; i < db->table_count; i++)
        persist_table(db, &(db->tables[i]));

    char filename[NAME_SIZE];
    int ret = snprintf(filename, NAME_SIZE, "%s/%s", PERSIST_DIR, db->name);
    assert(ret < NAME_SIZE);

    FILE *fp = fopen(filename, "w");
    fwrite(db, sizeof(db), 1, fp);
    fclose(fp);

}
