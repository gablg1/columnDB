%{
#include <stdio.h>
#include <math.h>
#include "../include/cs165_api.h"
#include "../include/utils.h"
#include "../include/message.h"
#include "../include/dbs.h"
#include "../include/list.h"
#include "../include/variables.h"

#include "lex.yy.h"

void yyerror(db_operator *op, message *send_msg, const char *msg);

%}

%parse-param {db_operator *op}
%parse-param {message *send_msg}

%union {
    char *str;
    int val;
    struct list *list;
    struct MaybeInt maybe_val;
    void *ptr;
}

/* _T is needed for conflict with #define */
%token CREATE
%token DB
%token TBL
%token COL
%token IDX
%token MIN_T
%token AVG
%token ADD
%token SUB
%token MAX_T
%token BEGIN_COMMENT
%token SORTED_T
%token BTREE_T
%token UNSORTED_T
%token PRIMARY_T
%token REL_INSERT
%token SELECT
%token HASHJOIN
%token SELECTV
%token FETCH
%token TUPLE
%token LOAD
%token SHUTDOWN
%token NULL_T

%token <str> WORD
%token <str> DOTTED_WORD
%token <val> INT
%type <str> name
%type <str> quoted_name
%type <maybe_val> maybe_int
%type <val> index_type
%type <list> arb_ints
%type <ptr> db
%type <ptr> tbl
%type <ptr> col
%type <ptr> var_or_col
%type <ptr> var
%type <ptr> vector_var


%%
line: expression
;


expression: query
;

db : name {
        //  we look to see if the db_name exists
        db *db = get_db_by_name($1);

        // if it doesn't, we terminate parsing immediately
        if (db == NULL) {
            op->type = ERROR_OP;
            add_payload(send_msg, "Could not find db %s", $1);
            YYERROR;
        }
        $$ = db;
   }
;

tbl : name {
        char *tbl_name = $1;
        char *db_name = strsep(&tbl_name, ".");
        if (tbl_name == NULL) {
            op->type = ERROR_OP;
            add_payload(send_msg, "Format of table name should be <db>.<table>");
            YYERROR;
        }

        //  we look to see if the name exists
        table *tbl = get_table_by_name(db_name, tbl_name);

        // if it doesn't, we terminate parsing immediately
        if (tbl == NULL) {
            op->type = ERROR_OP;
            add_payload(send_msg, "Could not find table %s in db %s", tbl_name, db_name);
            YYERROR;
        }
        $$ = tbl;
   }
;

col : name {
        char *col_name = $1;
        char *db_name = strsep(&col_name, ".");
        char *tbl_name = strsep(&col_name, ".");

        if (col_name == NULL) {
            op->type = ERROR_OP;
            add_payload(send_msg, "Format of column name should be <db>.<table>.<column>");
            YYERROR;
        }

        //  we look to see if the name exists
        column *col = get_column_by_name(db_name, tbl_name, col_name);

        // if it doesn't, we terminate parsing immediately
        if (col == NULL) {
            op->type = ERROR_OP;
            add_payload(send_msg, "Could not find column %s in table %s of db %s", col_name, tbl_name, db_name);
            YYERROR;
        }
        $$ = col;
   }
;


var_or_col: name {
          // this will always return a vector * no matter if var or col
          char *name = $1;
          if (strchr(name, '.')) {
                char *col_name = $1;
                char *db_name = strsep(&col_name, ".");
                char *tbl_name = strsep(&col_name, ".");

                if (col_name == NULL) {
                    op->type = ERROR_OP;
                    add_payload(send_msg, "Format of column name should be <db>.<table>.<column>");
                    YYERROR;
                }

                //  we look to see if the name exists
                column *col = get_column_by_name(db_name, tbl_name, col_name);

                // if it doesn't, we terminate parsing immediately
                if (col == NULL) {
                    op->type = ERROR_OP;
                    add_payload(send_msg, "Could not find column %s in table %s of db %s", col_name, tbl_name, db_name);
                    YYERROR;
                }
                $$ = col->vector;
          }
          else {
                variable *v = get_var_by_name($1);
                if (v == NULL) {
                    op->type = ERROR_OP;
                    add_payload(send_msg, "Could not find variable %s", $1);
                    YYERROR;
                }
                assert(v->type == VECTOR_N);
                $$ = v->v;
          }
          }

var : name {
        variable *v = get_var_by_name($1);
        if (v == NULL) {
            op->type = ERROR_OP;
            add_payload(send_msg, "Could not find variable %s", $1);
            YYERROR;
        }
        $$ = v;
    }
;

vector_var : var {
           variable *var = $1;
           $$ = var->v;
    }
;

query: CREATE '(' DB ',' quoted_name ')'
        {
            char *db_name = $5;
            db *found = get_db_by_name(db_name);
            if (found != NULL) {
                op->type = ERROR_OP;
                add_payload(send_msg, "DB %s already exists", db_name);
                YYERROR;
            }

            // Creating a DB needs no query plan
            op->type = NOOP;

            // get_next_allocated_db grabs a db from a global structure in dbs.c
            db *new = get_next_allocated_db();
            status st = create_db(db_name, &new);
            if (st.code == OK) {
                add_payload(send_msg, "Db %s created", db_name);
            } else {
                op->type = ERROR_OP;
                add_payload(send_msg, "Creation of db %s failed", db_name);
            }

        }
     | CREATE '(' TBL ',' quoted_name ',' db ',' INT ')'
        {
            char *tbl_name = $5;
            db *db = $7;
            int cols = $9;

            // Creating a DB needs no query plan
            op->type = NOOP;

            table *found = get_table_from_db_by_name(db, tbl_name);
            if (found != NULL) {
                op->type = ERROR_OP;
                add_payload(send_msg, "Table %s already exists", tbl_name);
                YYERROR;
            }


            // create_db will allocate the memory of the db for us
            status st = create_table(db, tbl_name, cols);
            if (st.code == OK) {
                add_payload(send_msg, "Table %s created", tbl_name);
            } else {
                op->type = ERROR_OP;
                add_payload(send_msg, "Creation of table failed");
            }
        }
     | CREATE '(' COL ',' quoted_name ',' tbl ',' index_type ')'
        {
            char *col_name = $5;
            table *tbl = $7;
            IndexType type = $9;

            // Creating a column needs no query plan
            op->type = NOOP;

            column *found = get_column_from_table_by_name(tbl, col_name);
            if (found != NULL) {
                op->type = ERROR_OP;
                add_payload(send_msg, "Column %s already exists", col_name);
                YYERROR;
            }

            status st = create_column(tbl, col_name, type);
            if (st.code == OK) {
                add_payload(send_msg, "Column %s created successfuly", col_name);
            } else {
                op->type = ERROR_OP;
                add_payload(send_msg, "Creation of column failed");
            }
        }
     | CREATE '(' IDX ',' col ',' index_type ')'
        {
            column *col = $5;
            IndexType type = $7;

            // Creating a column needs no query plan
            op->type = NOOP;

            if (col->index.type != UNSORTED) {
                add_payload(send_msg, "Index already exists");
                YYERROR;
            }

            status st = create_index(col, type);
            if (st.code == OK) {
                add_payload(send_msg, "Index created successfuly");
            } else {
                op->type = ERROR_OP;
                add_payload(send_msg, "Creation of index failed");
            }

        }
     | REL_INSERT '(' tbl ',' arb_ints ')' {
            table *tbl = $3;
            list *root = $5;

            if (tbl->col_count != root->length) {
                op->type = ERROR_OP;
                add_payload(send_msg, "Wrong number of values. Got %d, but needed %d", root->length, tbl->col_count);
                YYERROR;
            }
            op->type = INSERT;
            op->values1 = root;
            op->tbl = tbl;
     }
     | name '=' SELECTV '(' vector_var ',' vector_var ',' maybe_int ',' maybe_int ')' {
            char *var_name = $1;
            vector *pos_vec = $5;
            vector *val_vec = $7;
            MaybeInt low = $9;
            MaybeInt high = $11;

            op->type = NOOP;

            // select returns a vector of positions
            vector *v = select_two(pos_vec, val_vec, low, high);

            // now we just give a name and add it to our pool of variables
            add_vector_var(v, var_name);

            add_payload(send_msg, "Selected %d positions succesfully", v->length);

     }
     | name '=' SELECT '(' col ',' maybe_int ',' maybe_int ')' {
            char *var_name = $1;
            column *col = $5;
            MaybeInt low = $7;
            MaybeInt high = $9;

            op->type = NOOP;

            // select_one returns a vector of positions
            vector *v = select_one(col, low, high);

            // now we just give a name and add it to our pool of variables
            add_vector_var(v, var_name);

            add_payload(send_msg, "Selected %d positions succesfully", v->length);
     }
     | name ',' name '=' HASHJOIN '(' vector_var ',' var_or_col ',' vector_var ',' var_or_col ')' {
            char *var1_name = $1;
            char *var2_name = $3;
            vector *positions1 = $7;
            vector *values1 = $9;
            vector *positions2 = $11;
            vector *values2 = $13;

            vector *r1, *r2;
            hashjoin(&r1, &r2, positions1, values1, positions2, values2);

            add_vector_var(r1, var1_name);
            add_vector_var(r2, var2_name);
            add_payload(send_msg, "Joined %d positions succesfully", r1->length);
     }
     | name '=' FETCH '(' col ',' var ')' {
            char *var_name = $1;
            column *col = $5;
            variable *vn = $7;
            vector *pos_vec = vn->v;

            op->type = NOOP;

            // fetch returns a vector of values
            vector *v = fetch(col->vector, pos_vec);

            // now we just give a name and add it to our pool of variables
            add_vector_var(v, var_name);

            add_payload(send_msg, "Fetched %d values succesfully", v->length);
     } | name '=' MIN_T '(' var_or_col ')' {
            char *var_name = $1;
            vector *val_vec = $5;
            op->type = NOOP;
            int m = min(val_vec);
            add_int_var(m, var_name);
            add_payload(send_msg, "Minimum of %d calculated", m);
    } | name '=' MAX_T '(' var_or_col ')' {
            char *var_name = $1;
            vector *val_vec = $5;
            op->type = NOOP;
            int m = max(val_vec);
            add_int_var(m, var_name);
            add_payload(send_msg, "Maximum of %d calculated", m);
     } | name '=' AVG '(' var_or_col ')' {
            char *var_name = $1;
            vector *v = $5;
            op->type = NOOP;

            // avg is supported only for vector variables
            double av = avg(v);
            add_float_var(av, var_name);
            add_payload(send_msg, "Average of %f calculated", av);
     } | name '=' ADD '(' var_or_col ',' var_or_col ')' {
            char *var_name = $1;
            vector *v1 = $5;
            vector *v2 = $7;
            op->type = NOOP;

            vector *sum = add(v1, v2);
            add_vector_var(sum, var_name);
            add_payload(send_msg, "Sum of vectors calculated");
    } | name '=' SUB '(' var_or_col ',' var_or_col ')' {
            char *var_name = $1;
            vector *v1 = $5;
            vector *v2 = $7;
            op->type = NOOP;

            vector *diff = sub(v1, v2);
            add_vector_var(diff, var_name);
            add_payload(send_msg, "Difference of vectors calculated");
     } | TUPLE '(' var ')' {
            variable *var = $3;
            tuple(var, send_msg);
            op->type = NOOP;

     } | LOAD '(' quoted_name ')' {
        op->type = LOAD_OP;
     } | SHUTDOWN {
        op->type = SHUTDOWN_OP;
        send_msg->status = OK_SHUTDOWN;
        add_payload(send_msg, "Shutting down DB");
     }
;

arb_ints: INT {
            // here we create the linked list
            list *root = create_list();
            push_front($1, root);
            $$ = root;
         }
        | INT ',' arb_ints {
            // $3 is the root of the linked list
            list *root = $3;
            push_front($1, root);
            $$ = root;
        }
;

index_type: UNSORTED_T { $$ = UNSORTED; }
          | SORTED_T { $$ = SORTED; }
          | BTREE_T { $$ = BTREE; }
          | PRIMARY_T { $$ = PRIMARY; }

name: WORD { $$ = $1; }
;

quoted_name: '"' WORD '"' { $$ = $2; }

;

maybe_int: INT {
        MaybeInt n;
        n.present = 1;
        n.val = $1;
        $$ = n;
   } | NULL_T {
        MaybeInt n;
        n.present = 0;
        $$ = n;
        }

%%

void parse_dsl(char *str, db_operator *op, message *send_msg) {
    YY_BUFFER_STATE buf = yy_scan_string(str);

    send_msg->payload = NULL;
    yyparse(op, send_msg);
    yy_delete_buffer(buf);

    return;
}

void yyerror(db_operator *op, message *send_msg, const char *msg) {
    op->type = ERROR_OP;
    add_payload(send_msg, "Unknown command");
}

int yywrap(void) { return 1; }
