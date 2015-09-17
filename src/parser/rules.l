%{
#include <stdio.h>
#include "grammar.tab.h"

extern void yyerror(char *);
extern int yyparse(void);

%}

DIGIT [0-9]
LETTER [A-Za-z]
SYMBOL [_-]

%%

[\t\n ]     /* spaces just signify end of token */
;           { return END_OF_QUERY; }
*           { return ALL; }

.+          { return WORD; } /* A combination of everything else is a WORD */

%%

