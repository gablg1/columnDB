/*
Copyright (c) 2015 Harvard University - Data Systems Laboratory (DASLab)
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CS165_H
#define CS165_H

#include <stdlib.h>

/**
 * EXTRA
 * DataType
 * Flag to mark what type of data is held in the struct.
 * You can support additional types by including this enum and using void*
 * in place of int* in db_operator simliar to the way IndexType supports
 * additional types.
 **/
/**
typedef enum DataType {
     INT,
     LONG,
     // Others??
} DataType;
**/


/**
 * IndexType
 * Flag to identify index type. Defines an enum for the different possible column indices.
 * Additonal types are encouraged as extra.
 **/
typedef enum IndexType {
    SORTED,
    B_PLUS_TREE,
} IndexType;

/**
 * column_index
 * Defines a general column_index structure, which can be used as a sorted
 * index or a b+-tree index.
 * - type, the column index type (see enum index_type)
 * - index, a pointer to the index structure. For SORTED, this points to the
 *       start of the sorted array. For B+Tree, this points to the root node.
 *       You will need to cast this from void* to the appropriate type when
 *       working with the index.
 **/
typedef struct column_index {
    IndexType type;
    void* index;
} column_index;

/**
 * column
 * Defines a column structure, which is the building block of our column-store.
 * Operations can be performed on one or more columns.
 * - name, the string associated with the column. Column names must be unique
 *       within a table, but columns from different tables can have the same
 *       name.
 * - data, this is the raw data for the column. Operations on the data should
 *       be persistent.
 * - index, this is an [opt] index built on top of the column's data.
 *
 * NOTE: We do not track the column length in the column struct since all
 * columns in a table should share the same length. Instead, this is
 * tracked in the table (length).
 **/
typedef struct column {
    char* name;
    int* data;
    column_index *index;
} column;

/**
 * table
 * Defines a table structure, which is composed of multiple columns.
 * We do not require you to dynamically manage the size of your tables,
 * although you are free to append to the struct if you would like to (i.e.,
 * in clude a size_t table_size).
 * name, the name associated with the table. Table names must be unique
 *     within a database, but tables from different databases can have the same
 *     name.
 * - col_count, the number of columns in the table
 * - col, this is the pointer to an array of columns contained in the table.
 * - length, the size of the columns in the table.
 **/
typedef struct table {
    char* name;
    size_t col_count;
    column* col;
    size_t length;
} table;

/**
 * db
 * Defines a database structure, which is composed of multiple tables.
 * - name: the name of the associated database.
 * - table_count: the number of tables in the database.
 * - tables: the pointer to the array of tables contained in the db.
 **/
typedef struct db {
    char* name;
    size_t table_count;
    table* tables;
} db;

/**
 * Error codes used to indicate the outcome of an API call
 **/
typedef enum StatusCode {
  /* The operation completed successfully */
  OK,
  /* There was an error with the call.
  */
  ERROR,
} StatusCode;

// status declares an error code and associated message
typedef struct status {
    StatusCode code;
    char* error_message;
} status;

// Defines a comparator flag between two values.
typedef enum ComparatorType {
    LESS_THAN = 1,
    GREATER_THAN = 2,
    EQUAL = 4,
} ComparatorType;

/**
 * A Junction defines the relationship between comparators.
 * In cases where we have more than one comparator, e.g. A.a <= 5 AND A.b > 3,
 * A NONE Junction defines the END of a comparator junction.
 *
 * Using the comparator struct defined below, we would represent our example using:
 *     // This represents the sub-component (A.b > 3)
 *     comparator f_b;
 *     f_b.p_val = 3; // Predicate values
 *     f_b.type = GREATER_THAN;
 *     f_b.mode = NONE;
 *
 *     // This represents the entire comparator
 *     comparator f;
 *     f.value = 5;
 *     f.mode = LESS_THAN | EQUAL;
 *     f.next_comparator = &f_b;
 * For chains of more than two Juntions, left associative: "a | b & c | d"
 * evaluated as "(((a | b) & c) | d)".
 **/

typedef enum Junction {
    NONE,
    OR,
    AND,
} Junction;

/**
 * comparator
 * A comparator defines one or more comparasons.
 * See the example in Junction
 **/
typedef struct comparator {
    int p_val;
    column *col;
    ComparatorType type;
    struct comparator *next_comparator;
    Junction mode;
} comparator;

typedef struct result {
    size_t num_tuples;
    int *payload;
} result;

typedef enum Aggr {
    MIN,
    MAX,
    SUM,
    AVG,
    CNT,
} Aggr;

typedef enum OperatorType {
    SELECT,
    PROJECT,
    HASH_JOIN,
    INSERT,
    DELETE,
    UPDATE,
    AGGREGATE,
} OperatorType;

/**
 * db_operator
 * The db_operator defines a database operation.  Generally, an operation will
 * be applied on column(s) of a table (SELECT, PROJECT, AGGREGATE) while other
 * operations may involve multiple tables (JOINS). The OperatorType defines
 * the kind of operation.
 *
 * In UNARY operators that only require a single table, only the variables
 * related to table1/column1 will be used.
 * In BINARY operators, the variables related to table2/column2 will be used.
 *
 * If you are operating on more than two tables, you should rely on separating
 * it into multiple operations (e.g., if you have to project on more than 2
 * tables, you should select in one operation, and then create separate
 * operators for each projection).
 *
 * Example query:
 * SELECT a FROM A WHERE A.a < 100;
 * db_operator op1;
 * op1.table1 = A;
 * op1.column1 = b;
 *
 * filter f;
 * f.value = 100;
 * f.type = LESS_THAN;
 * f.mode = NONE;
 *
 * op1.comparator = f;
 **/
typedef struct db_operator {
    // Flag to choose operator
    OperatorType type;

    // Used for every operator
    table** tables;
    column** columns;

    // Internmediaties used for PROJECT, DELETE, HASH_JOIN
    int *pos1;
    // Needed for HASH_JOIN
    int *pos2;

    // For insert/delete operations, we only use value1;
    // For update operations, we update value1 -> value2;
    int *value1;
    int *value2;

    // This includes several possible fields that may be used in the operation.
    Aggr agg;
    comparator* c;

} db_operator;

typedef enum OpenFlags {
    CREATE = 1,
    LOAD = 2,
} OpenFlags;

/* OPERATOR API*/
/**
 * open_db(filename, db, flags)
 * Opens the file associated with @filename and loads its contents into @db.
 *
 * If flags | Create, then it should create a new db.
 * If flags | Load, then it should load the db with the incoming data.
 *
 * Note that the database in @filename MUST contain the same name as db->name
 * (if db != NULL). If not, then return an error.
 *
 * filename: the name associated with the DB file
 * db      : the pointer to db*
 * flags   : the flags indicating the create/load options
 * returns : a status of the operation.
 */
status open_db(const char* filename, db** db, OpenFlags flags);

/**
 * drop_db(db)
 * Drops the database associated with db.  You should permanently delete
 * the db and all of its tables/columns.
 *
 * db       : the database to be dropped.
 * returns  : the status of the operation.
 **/
status drop_db(db* db);

/**
 * sync_db(db)
 * Saves the current status of the database to disk.
 *
 * db       : the database to sync.
 * returns  : the status of the operation.
 **/
status sync_db(db* db);

/**
 * create_db(db_name, db)
 * Creates a database with the given database name, and stores the pointer in db
 *
 * db_name  : name of the database, must be unique.
 * db       : pointer to the db pointer. If *db == NULL, then create_db is
 *            responsible for allocating space for the db, else it should assume
 *            that *db points to pre-allocated space.
 * returns  : the status of the operation.
 *
 * Usage:
 *  db *database = NULL;
 *  status s = create_db("db_cs165", &database)
 *  if (s.code != OK) {
 *      // Something went wrong
 *  }
 **/
status create_db(const char* db_name, db** db);

/**
 * create_table(db, name, num_columns, table)
 * Creates a table named @name in @db with @num_columns, and stores the pointer
 * in @table.
 *
 * db          : the database in which to create the table.
 * name        : the name of the new table, must be unique in the db.
 * num_columns : the non-negative number of columns in the table.
 * table       : the pointer to the table pointer. If *table == NULL, then
 *               create_table is responsible for allocating space for a table,
 *               else it assume that *table points to pre-allocated space.
 * returns     : the status of the operation.
 *
 * Usage:
 *  // Assume you have a valid db* 'database'
 *  table* tbl = NULL;
 *  status s = create_table(database, "tbl_cs165", 4, &tbl)
 *  if (s.code != OK) {
 *      // Something went wrong
 *  }
 **/
status create_table(db* db, const char* name, size_t num_columns, table** table);

/**
 * drop_table(db, table)
 * Drops the table from the db.  You should permanently delete
 * the table and all of its columns.
 *
 * db       : the database that contains the table.
 * table    : the table to be dropped.
 * returns  : the status of the operation.
 **/
status drop_table(db* db, table* table);

/**
 * create_column(table, name, col)
 * Creates a column named @name in @table, and stores the pointer in @col.
 *
 * table   : the table in which to create the column.
 * name    : the name of the column, must be unique in the table.
 * col     : the pointer to the column pointer. If *col == NULL, then
 *           create_column is responsible for allocating space for a column*,
 *           else it should assume that *col points to pre-allocated space.
 * returns : the status of the operation.
 *
 * Usage:
 *  // Assume that you have a valid table* 'tbl';
 *  column* col;
 *  status s = create_column(tbl, 'col_cs165', &col)
 *  if (s.code != OK) {
 *      // Something went wrong
 *  }
 **/
status create_column(table *table, const char* name, column** col);

/**
 * create_index(col, type)
 * Creates an index for @col of the given IndexType. It stores the created index
 * in col->index.
 *
 * col      : the column for which to create the index.
 * type     : the enum representing the index type to be created.
 * returns  : the status of the operation.
 **/
status create_index(column* col, IndexType type);

status insert(column *col, int data);
status delete(column *col, int *pos);
status update(column *col, int *pos, int new_val);
status col_scan(comparator *f, column *col, result **r);
status index_scan(comparator *f, column *col, result **r);

/* Query API */
status query_prepare(const char* query, db_operator** op);
status query_execute(db_operator* op, result** results);


#endif /* CS165_H */

