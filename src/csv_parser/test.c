#include "lex.yy.h"
#include "grammar.tab.h"
#include "../include/cs165_api.h"

int parse_sql(char *str, db_operator *op);

int main(void) {
    db_operator op;
    parse_sql("create(db, \"awesome\")", &op);
}
