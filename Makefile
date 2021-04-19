CC = gcc
CFLAG = 
L = flex
B = bison
BFLAG = -vd

all: parser

parser: lex bison
	$(CC) $(CFLAG) lex.yy.c tinylex.tab.c symtab.c -o parser

lex: tinyc.l symtab.h tinyc.l
	$(L) $(LFLAG) tinyc.l

bison: tinylex.y symtab.h tinyc.l
	$(B) $(BFLAG) tinylex.y

clean:
	rm parser lex.yy.c tinylex.tab.c tinylex.tab.h *.output
