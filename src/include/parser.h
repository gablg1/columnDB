#ifndef PARSER_H__
#define PARSER_H__

#include "cs165_api.h"
#include "dsl.h"
#include "utils.h"

// This parses the command string and then update the db_operator if it requires
// a specific query plan to be executed.
// This can automatically run jobs that don't require a query plan,
// (e.g., create_db, create_tbl, create_col);
//
// Usage: parse_command_string(input_query, commands, operator);
status parse_command_string(char* str, dsl** commands, db_operator* op);

#endif // PARSER_H__
