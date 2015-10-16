all:
	flex --header-file=lex.yy.h rules.l
	bison -d grammar.y
	gcc -c grammar.tab.c lex.yy.c -ggdb3

test: all
	gcc -o test test.c grammar.tab.o lex.yy.o -ggdb3

clean:
	rm grammar.tab.h lex.yy.c grammar.tab.c a.out *.o
