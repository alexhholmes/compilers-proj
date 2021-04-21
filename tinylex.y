%{
#include "tinylex.tab.h"
#include "symtab.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

extern int num_line;
extern bool declared;
extern int yylex();
extern void yyerror(char *s);
%}

/* Keywords */   
%token IF 
%token ELSE
%token WHILE   
%token CHAR
%token INT
%token FLOAT
%token RETURN
%token VOID

/* Pairs of tokens */
%token LTPAR     /* ( */
%token RTPAR     /* ) */
%token LTBRACE   /* { */
%token RTBRACE   /* } */    

/* Basic separators */
%token SEMICOLON
%token COMMA

/* Math operators */
%token PLUS
%token MINUS
%token MULTIPLY
%token DIVIDE

/* Equality and difference */
%token ASSIGNMENT  /* = */
%token EQUAL       /* == */
%token NOT_EQUAL   /* != */

/* Logical operators */
%token GT      /* > */
%token LT      /* < */
%token GTE     /* >= */
%token LTE     /* <= */

%token PTR     /* & */

%token IDENTIFIER
%token CHAR_CONST
%token INT_CONST
%token FLOAT_CONST
%token STRING_CONST

%left ASSIGNMENT
%left EQUAL NOT_EQUAL
%left GT LT GTE LTE
%left PLUS MINUS
%left MULTIPLY DIVIDE
%left PTR

%nonassoc NO_ELSE
%nonassoc ELSE

%union {
    int int_value;
    char char_value;
    float float_value;
    char *string_value;

    struct Symbol *symbol_item;
}

%% 

program: func_deflist
    ;

primary_exp: constant
    | IDENTIFIER
    | func_call
    | LTPAR exp RTPAR
    ;

constant: INT_CONST
    | FLOAT_CONST
    | STRING_CONST
    | CHAR_CONST
    ;

type: INT
    | FLOAT
    | CHAR
    ;

func_arglist: PTR IDENTIFIER
    | exp
    | exp COMMA func_arglist
    | PTR IDENTIFIER COMMA func_arglist
    ;

func_call: IDENTIFIER LTPAR x;

/* TODO */
x: func_arglist RTPAR | RTPAR;

unary_exp: primary_exp 
    | PLUS unary_exp
    | MINUS unary_exp
    ;

mult_exp: unary_exp
    | mult_exp MULTIPLY unary_exp
    | mult_exp DIVIDE unary_exp
    ;

add_exp: mult_exp
    | add_exp PLUS add_exp
    | add_exp MINUS add_exp
    ;

comp_exp: add_exp
    | add_exp LT add_exp
    | add_exp LTE add_exp
    | add_exp GT add_exp
    | add_exp GTE add_exp
    ;

exp: comp_exp
    | comp_exp EQUAL comp_exp
    | comp_exp NOT_EQUAL comp_exp
    ;

/* statements */

assign_st: IDENTIFIER ASSIGNMENT exp SEMICOLON
    ;

if_st: IF LTPAR exp RTPAR st %prec NO_ELSE
  | IF LTPAR exp RTPAR st ELSE st
  ;

while_st: WHILE LTPAR exp RTPAR st
    ;

ret_st: RETURN SEMICOLON
    | RETURN exp SEMICOLON
    ;

st_list: /* epsilon */ 
    | st st_list
    ;

block_st: LTBRACE st_list RTBRACE
    ;

empty_st: SEMICOLON
    ;

st: assign_st
    | if_st
    | while_st
    | ret_st
    | block_st
    | empty_st
    | func_call SEMICOLON
    ;

/* functions */

return_type: VOID
    | type 
    ;

var_def: type { declared = true; } IDENTIFIER { declared = false; } ASSIGNMENT constant SEMICOLON
    ;

var_deflist: /* epsilon */
    | var_def var_deflist
    ;

func_stlist: ret_st
    | st func_stlist
    ;

func_body: var_deflist func_stlist
    ;

func_param: { declared = true; } type IDENTIFIER { declared = false; }
    ;

func_paramlist: func_param
    | func_param COMMA func_param
    ;

func_def_tail: LTPAR func_paramlist RTPAR LTBRACE func_body RTBRACE
    | LTPAR VOID RTPAR LTBRACE func_body RTBRACE
    ;

func_def: return_type { declared = true; } IDENTIFIER { declared = false; } func_def_tail
    ;

/* programs */

func_deflist: /* epsilon */
    | func_deflist func_def 
    ;

%%
void yyerror(char *s) {
    printf("\n Error: %s on line %d\n", s, num_line);
}

int yywrap() {
    // Stop scanning at EOF
    return 1;
}

int main(void) {
    yyparse();

    #ifdef DEBUG
    print_symtab();
    #endif

    return 0;
}