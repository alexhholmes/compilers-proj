%{
#include "tinylex.tab.h"
#include "symtab.h"

#include <math.h>

int num_line = 1;
%}

DIGIT   [0-9]
LETTER  [a-zA-Z]
ESCAPE_CHAR [\\'"nt]

IDENTIFIER ({LETTER}|_)({LETTER}|{DIGIT}|_)*
CHAR (\'\\{ESCAPE_CHAR}\')|(\'{LETTER}\')
INTEGER {DIGIT}+
FLOAT {DIGIT}+\.{DIGIT}+
STRING \"(\\.|[^"\\])*\"

%%

"if"                    { return(IF); }
"else"                  { return(ELSE); }
"while"                 { return(WHILE); }
"char"                  { return(CHAR); }
"int"                   { return(INT); }
"float"                 { return(FLOAT); }
"return"                { return(RETURN); }
"void"                  { return(VOID); }

"("                     { return(LTPAR); }
")"                     { return(RTPAR); }
"{"                     { return(LTBRACE); }
"}"                     { return(RTBRACE); }


"+"                     { return(PLUS); }
"-"                     { return(MINUS); }
"*"                     { return(MULTIPLY); }
"/"                     { return(DIVIDE); }

"="                     { return(ASSIGNMENT); }

"=="                    { return(EQUAL); }
"!="                    { return(NOT_EQUAL); }
">"                     { return(GT); }
"<"                     { return(LT); }
">="                    { return(GTE); }
"<="                    { return(LTE); }

"&"                     { return(PTR); }

";"                     { return(SEMICOLON); }

","                     { return(COMMA); }

{IDENTIFIER}            { append_sym(yytext, strlen(yytext), UNDEF);
                          yylval.symbol_item = lookup_sym(yytext);
                          return(IDENTIFIER); }

{CHAR}                  { yylval.char_value = yytext[1];
                          return(CHAR_CONST); }

{INTEGER}               { yylval.int_value = atoi(yytext);                               
                          return(INT_CONST); }

{FLOAT}                 { yylval.float_value = atof(yytext);                                
                          return(FLOAT_CONST); }

{STRING}                { yylval.string_value = yytext;
                          return (STRING_CONST); }

[ \t]                   /* skip whitespace */
[\n\r]                  { num_line++; }

%%
