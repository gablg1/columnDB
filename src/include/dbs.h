#include "message.h"

db *get_next_allocated_db(void);

db *get_db_by_name(const char *lookup_name, message *send_msg);

table *get_table_by_name(const char *db_name, const char *tbl_name, message *send_msg);

column *get_column_by_name(const char *db_name, const char *tbl_name, const char *col_name, message *send_msg);

void drop_dbs(void);
