#include "parser.h"

#include <regex.h>
#include <string.h>
#include <ctype.h>

#include "db.h"

// Trims the whitespace
char* trim(char *str);

// Prototype for Helper function that executes that actual parsing after
// parse_command_string has found a matching regex.
status parse_dsl(char* str, dsl* d, db_operator* op);

// Finds a possible matching DSL command by using regular expressions.
// If it finds a match, it calls parse_command to actually process the dsl.
status parse_command_string(char* str, dsl** commands, db_operator* op)
{
    log_info("Parsing: %s", str);

    // Trim the string of any spaces.
    char* trim_str = trim(str);

    // Create a regular expression to parse the string
    regex_t regex;
    int ret;

    // Track the number of matches; a string must match all
    int n_matches = 1;
    regmatch_t m;

    for (int i = 0; i < NUM_DSL_COMMANDS; ++i) {
        dsl* d = commands[i];
        if (regcomp(&regex, d->c, REG_EXTENDED) != 0) {
            log_err("Could not compile regex\n");
        }

        // Bind regular expression associated with the string
        ret = regexec(&regex, trim_str, n_matches, &m, 0);

        // If we have a match, then figure out which one it is!
        if (ret == 0) {
            log_info("Found Command: %d\n", i);
            // Here, we actually strip the command as appropriately
            // based on the DSL to get the variable names.
            return parse_dsl(trim_str, d, op);
        }
    }

    // Nothing was found!
    status s;
    s.code = ERROR;
    return s;
}

status parse_dsl(char* str, dsl* d, db_operator* op)
{
    // Use the delimiters to parse out the string
    char open_paren[2] = "(";
    char close_paren[2] = ")";
    char delimiter[2] = ",";
    // char end_line[2] = "\n";
    // char eq_sign[2] = "=";

    if (d->g == CREATE_DB) {
        // Create a working copy, +1 for '\0'
        char* str_cpy = malloc(strlen(str));
        strncpy(str_cpy, str, strlen(str));

        // This gives us everything inside the (db, <db_name>)
        strtok(str_cpy, open_paren);
        char* args = strtok(NULL, close_paren);

        // This gives us "db", but we don't need to use it
        char* db_indicator = strtok(args, delimiter);
        (void) db_indicator;

        // This gives us <db_name>
        char* db_name = strtok(NULL, delimiter);

        log_info("create_db(%s)\n", db_name);

        // Here, we can create the DB using our parsed info!
        db* db1 = NULL;
        status s = create_db(db_name, &db1);
        if (s.code != OK) {
            // Something went wrong
        }

        // TODO(USER): You must track your variable in a variable pool now!
        // This means later on when I refer to <db_name>, I should get this
        // same db*.  You can do this in many ways, including associating
        // <db_name> -> db1

        // Free the str_cpy
        free(str_cpy);

        // No db_operator required, since no query plan
        (void) op;
        status ret;
        ret.code = OK;
        return ret;
    } else if (d->g == CREATE_TABLE) {
        // Create a working copy, +1 for '\0'
        char* str_cpy = malloc(strlen(str) + 1);
        strncpy(str_cpy, str, strlen(str) + 1);

        // This gives us everything inside the (table, <tbl_name>, <db_name>, <count>)
        strtok(str_cpy, open_paren);
        char* args = strtok(NULL, close_paren);

        // This gives us "table"
        char* tbl_indicator = strtok(args, delimiter);
        (void) tbl_indicator;

        // This gives us <tbl_name>, we will need this to create the full name
        char* tbl_name = strtok(NULL, delimiter);

        // This gives us <db_name>, we will need this to create the full name
        char* db_name = strtok(NULL, delimiter);

        // Generate the full name using <db_name>.<tbl_name>
        char full_name[strlen(tbl_name) + strlen(db_name)];
        strncat(full_name, db_name, strlen(db_name));
        strncat(full_name, ".", 1);
        strncat(full_name, tbl_name, strlen(tbl_name));

        // This gives us count
        char* count_str = strtok(NULL, delimiter);
        int count = 0;
        if (count_str != NULL) {
            count = atoi(count_str);
        }
        (void) count;

        log_info("create_table(%s, %s, %d)\n", full_name, db_name, count);

        // Here, we can create the table using our parsed info!
        // TODO(USER): You MUST get the original db* associated with <db_name>
        // db* db1 = NULL;

        // TODO(USER): Uncomment this section after you're able to grab the db1
        // table* tbl1 = NULL;
        // status s = create_table(db1, full_name, count, &tbl1);
        // if (s.code != OK) {
        //     // Something went wrong
        // }

        // TODO(USER): You must track your variable in a variable pool now!
        // This means later on when I refer to <full_name>, I should get this
        // same table*.  You can do this in many ways, including associating
        // <full_name> -> tbl1

        // Free the str_cpy
        free(str_cpy);

        // No db_operator required, since no query plan
        status ret;
        ret.code = OK;
        return ret;
    } else if (d->g == CREATE_COLUMN) {
        // Create a working copy, +1 for '\0'
        char* str_cpy = malloc(strlen(str) + 1);
        strncpy(str_cpy, str, strlen(str) + 1);

        // This gives us everything inside the (col, <col_name>, <tbl_name>, unsorted)
        strtok(str_cpy, open_paren);
        char* args = strtok(NULL, close_paren);

        // This gives us "col"
        char* col_indicator = strtok(args, delimiter);
        (void) col_indicator;

        // This gives us <col_name>, we will need this to create the full name
        char* col_name = strtok(NULL, delimiter);

        // This gives us <tbl_name>, we will need this to create the full name
        char* tbl_name = strtok(NULL, delimiter);

        // Generate the full name using <db_name>.<tbl_name>
        char full_name[strlen(tbl_name) + strlen(col_name) + 1];
        strncat(full_name, tbl_name, strlen(tbl_name));
        strncat(full_name, ".", 1);
        strncat(full_name, col_name, strlen(col_name));

        // This gives us the "unsorted"
        char* sorting_str = strtok(NULL, delimiter);
        (void) sorting_str;

        log_info("create_column(%s, %s, %s)\n", full_name, tbl_name, sorting_str);

        // Here, we can create the column using our parsed info!
        // TODO(USER): You MUST get the original table* associated with <tbl_name>
        // table* table1 = NULL;

        // TODO(USER): Uncomment this section after you're able to grab the tbl1
        // column* col1 = NULL;
        // status s = create_column(tbl1, full_name, &col1);
        // if (s.code != OK) {
        //     // Something went wrong
        // }

        // TODO(USER): You must track your variable in a variable pool now!
        // This means later on when I refer to <full_name>, I should get this
        // same col*.  You can do this in many ways, including associating
        // <full_name> -> col1

        // Free the str_cpy
        free(str_cpy);

        // No db_operator required, since no query plan
        status ret;
        ret.code = OK;
        return ret;
    }

    // Should have been caught earlier...
    status fail;
    fail.code = ERROR;
    return fail;
}

// Trims the whitespace
char* trim(char *str)
{
    int length = strlen(str);
    int current = 0;
    for (int i = 0; i < length; ++i) {
        if (!isspace(str[i])) {
            str[current++] = str[i];
        }
    }

    // Write new null terminator
    str[current] = 0;
    return str;
}
