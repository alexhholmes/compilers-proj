CC = gcc
CFLAG = 
L = flex
B = bison
BFLAG = -vd

all: parser

parser: lex bison
	$(CC) $(CFLAG) lex.yy.c tinylex.tab.c -o parser

lex: tinyc.l
	$(L) $(LFLAG) tinyc.l

bison: tinylex.y
	$(B) $(BFLAG) tinylex.y

clean:
	rm parser lex.yy.c tinylex.tab.c tinylex.tab.h *.output
