#include <stdio.h>
#include <assert.h>

#include "cs165_api.h"

#define PERSIST_FOLDER "persist"

void persist_column(db *db, table *tbl, column *col) {
    assert(db != NULL && tbl != NULL && col != NULL);
    char filename[NAME_SIZE];
    int ret = snprintf(filename, NAME_SIZE, "%s/%s.%s.%s", PERSIST_FOLDER, db->name, tbl->name, col->name);
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
    int ret = snprintf(filename, NAME_SIZE, "%s/%s.%s", PERSIST_FOLDER, db->name, tbl->name);
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
    int ret = snprintf(filename, NAME_SIZE, "%s/%s", PERSIST_FOLDER, db->name);
    assert(ret < NAME_SIZE);

    FILE *fp = fopen(filename, "w");
    fwrite(db, sizeof(db), 1, fp);
    fclose(fp);

}
