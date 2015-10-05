#include "dsl.h"

// Create Commands
// Matches: create(db, <db_name>);
const char* create_db_command = "^create\\(db\\,\\\"[a-zA-Z0-9_]+\\\"\\)";

// Matches: create(table, <table_name>, <db_name>, <col_count>);
const char* create_table_command = "^create\\(tbl\\,\\\"[a-zA-Z0-9_\\.]+\\\"\\,[a-zA-Z0-9_\\.]+\\,[0-9]+\\)";

// Matches: create(col, <col_name>, <tbl_var>, sorted);
const char* create_col_command_sorted = "^create\\(col\\,\\\"[a-zA-Z0-9_\\.]+\\\"\\,[a-zA-Z0-9_\\.]+\\,sorted)";

// Matches: create(col, <col_name>, <tbl_var>, unsorted);
const char* create_col_command_unsorted = "^create\\(col\\,\\\"[a-zA-Z0-9_\\.]+\\\"\\,[a-zA-Z0-9_\\.]+\\,unsorted)";


// TODO(USER): You will need to update the commands here for every single command you add.
dsl** dsl_commands_init(void)
{
    dsl** commands = calloc(NUM_DSL_COMMANDS, sizeof(dsl*));

    for (int i = 0; i < NUM_DSL_COMMANDS; ++i) {
        commands[i] = malloc(sizeof(dsl));
    }

    // Assign the create commands
    commands[0]->c = create_db_command;
    commands[0]->g = CREATE_DB;

    commands[1]->c = create_table_command;
    commands[1]->g = CREATE_TABLE;

    commands[2]->c = create_col_command_sorted;
    commands[2]->g = CREATE_COLUMN;

    commands[3]->c = create_col_command_unsorted;
    commands[3]->g = CREATE_COLUMN;

    return commands;
}
