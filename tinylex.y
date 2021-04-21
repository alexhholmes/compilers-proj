%{
#include "tinylex.tab.h"
#include "symtab.h"
#include "ast.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

extern int num_line;
extern bool declared;
extern int yylex();
extern void yyerror(char *s);
%}

/* Keywords */   
%token <val> IF 
%token <val> ELSE
%token <val> WHILE   
%token <val> CHAR
%token <val> INT
%token <val> FLOAT
%token <val> RETURN
%token <val> VOID

/* Pairs of tokens */
%token <val> LTPAR     /* ( */
%token <val> RTPAR     /* ) */
%token <val> LTBRACE   /* { */
%token <val> RTBRACE   /* } */    

/* Basic separators */
%token <val> SEMICOLON
%token <val> COMMA

/* Math operators */
%token <val> PLUS
%token <val> MINUS
%token <val> MULTIPLY
%token <val> DIVIDE

/* Equality and difference */
%token <val> ASSIGNMENT  /* = */
%token <val> EQUAL       /* == */
%token <val> NOT_EQUAL   /* != */

/* Logical operators */
%token GT      /* > */
%token LT      /* < */
%token GTE     /* >= */
%token LTE     /* <= */

%token PTR     /* & */

%token <symbol_item> IDENTIFIER
%token <val> CHAR_CONST
%token <val> INT_CONST
%token <val> FLOAT_CONST
%token <val> STRING_CONST

%left ASSIGNMENT
%left EQUAL NOT_EQUAL
%left GT LT GTE LTE
%left PLUS MINUS
%left MULTIPLY DIVIDE
%left PTR

%nonassoc NO_ELSE
%nonassoc ELSE

/* Non-terminal definitions */
%type <node> program
%type <node> func_deflist
%type <node> func_def
%type <node> func_body
%type <node> func_st_list
%type <node> ret_st
%type <node> st

/*
%type <node> primary_exp
%type <node> constant
%type <data_type> type
%type <node> func_arglist
%type <node> func_call
%type <node> unary_exp
%type <node> mult_exp
%type <node> add_exp
%type <node> comp_exp
%type <node> exp
%type <node> assign_st
%type <node> if_st
%type <node> while_st
%type <node> ret_st
%type <node> st_list
%type <node> block_st
%type <node> empty_st
%type <node> func_call_st
%type <node> st
%type <node> return_type
%type <node> var_def
%type <node> var_deflist
%type <node> func_st_list
%type <node> func_body
%type <param> func_param
%type <node> func_paramlist
%type <node> func_def_tail
*/

%union {
    Value value;
    struct Symbol *symbol_item;
    AST_Node *node;

    // For declarations
    int data_type;
    int const_type;

    // For parameters
    Param param;
}

%% 

program: func_deflist { ast_traversal($1); }
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

func_call: IDENTIFIER LTPAR func_arglist RTPAR
    | RTPAR;

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

func_call_st: func_call SEMICOLON
    ;

st: assign_st
    | if_st
    | while_st
    | ret_st
    | block_st
    | empty_st
    | func_call_st
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

func_st_list: ret_st
    | st func_st_list
    ;

func_body: var_deflist func_st_list
    {
        $$ = new_ast_node(AST_NODE, $1, $2);
    }
    ;

func_param: { declared = true; } type IDENTIFIER
    { 
        declared = false;
        $$ = def_param($2, $3->name, 0);
    }
    ;

func_paramlist: func_param
    { 
        $$ = new_ast_function_declaration_params(NULL, 0, $1);
    }
    | func_paramlist COMMA func_param
    {
        AST_Function_declaration_Params *temp = (AST_Function_declaration_Params*);
        $$ = new_ast_function_declaration_params(temp->parameters, temp->num_params, $3);
    }
    ;

func_def: return_type { declared = true; } IDENTIFIER { declared = false; } LTPAR func_paramlist RTPAR LTBRACE func_body RTBRACE
    {
        $$ = new_ast_function_declaration($1, $3, $6, $9);
    }
    | return_type { declared = true; } IDENTIFIER { declared = false; } LTPAR VOID RTPAR LTBRACE func_body RTBRACE
    {
        $$ = new_ast_function_declaration($1, $3, NULL, $6, %9);
    }
    ;

/* programs */

func_deflist: func_deflist func_def 
    {
        // Pass old AST_Function_Declarations to new_ast_function_declarations
        // to reallocate a larger declarations array.
        AST_Function_Declarations *temp = (AST_Function_Declarations*) $1;
        $$ = new_ast_function_declarations(temp->func_declarations, temp->func_declaration_count, $2);
    }
    | func_def
    {
        $$ = new_ast_function_declarations(NULL, 0, $1);
    }
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