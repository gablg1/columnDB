#include "cs165_api.h"

void persist_column(column *col) {
    (void) col;
    return;
}

void persist_table(table *tbl) {
    for (size_t i = 0; i < tbl->col_count; i++)
        persist_column(&(tbl->cols[i]));

}

void persist_db(db *db) {
    for (size_t i = 0; i < db->table_count; i++)
        persist_table(&(db->tables[i]));

}
