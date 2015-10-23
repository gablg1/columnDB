
FILE *persist_fopen(const char *restrict filename, const char *restrict mode);

void load_column(const char *filename, column *col);

void load_table(const char* filename, table *tbl);

void load_db(const char* filename, db *db);

void persist_column(db *db, table *tbl, column *col);

void persist_table(db *db, table *tbl);

void persist_db(db *db);
