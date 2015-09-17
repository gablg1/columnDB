%{
#include <stdio.h>
#include <math.h>
#include "lex.yy.h"
int yyparse();
void yyerror(char *);
%}

%union {
    int val;
    int index;
}

%type  <val>  exp

%token <val>  NUMERO
%token <index>  PABRE
%token <index>  PFECHA
%token <index>  NEWLINE

%left '-' '+'
%left '*' '/'
%right EXPONENCIACAO

%start input

%%
input:
     | input line
;

line:
      NEWLINE        { printf("Soma vazia\n"); }
      | exp NEWLINE  { printf ("%d\n", $1); }
;

exp:      NUMERO                            { $$ = $1; }
        | exp '+' exp                       { $$ = $1 + $3; }
        | exp '-' exp                       { $$ = $1 - $3; }
        | exp '*' exp                       { $$ = $1 * $3; }
        | exp '/' exp                       { $$ = $1 / $3; }
        | exp EXPONENCIACAO exp             { $$ = pow ($1, $3); }
        | PABRE exp PFECHA                  { $$ = $2; }
;
%%

void yyerror(char *str) { printf("Erro na análise\n"); }
int yywrap(void) { return 0; }
int main(void)
{
	YY_BUFFER_STATE yybuf = yy_scan_string("5 + 5\n");
	*(yybuf->yy_ch_buf + 6) = EOF;
    yyparse();

    return 0;
}
