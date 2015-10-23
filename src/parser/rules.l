%{
#include <stdio.h>
#include "../include/cs165_api.h"

#include "grammar.tab.h"

void comment(void);

//extern void yyerror(db_operator *op, char *);
//extern int yyparse(void);
%}

DIGIT [0-9]
LETTER [A-Za-z]
ALPHANUM_AND_SYMBOLS [A-Za-z0-9_/]
ALPHANUM_AND_SYMBOLS_AND_DOT [A-Za-z0-9_/.]
VALID_SYMBO [_]
SYMBOL [,();=]

%%

-- {comment();}
create {return CREATE;}
relational_insert {return REL_INSERT;}
select {return SELECT;}
fetch {return FETCH;}
load { return LOAD;}
shutdown { return SHUTDOWN; }
db { return DB; }
tbl { return TBL; }
col { return COL; }
unsorted { return UNSORTED_T; }
sorted {return SORTED_T; }


[\t ]     /* spaces just signify end of token */
"."|","|"("|")"|";"|"="|"\"" { return yytext[0]; }

null { return NULL_T; }
{DIGIT}+ {yylval.val = atoi(yytext); return INT;}


{ALPHANUM_AND_SYMBOLS_AND_DOT}+ { yylval.str = strdup(yytext); return WORD; } /* A combination of everything else
is a WORD */


%%


void comment(void) {
	char c;
	// we just go until the end of input
	while ((c = input()) != '\0' && c != '\n')
		;
}
