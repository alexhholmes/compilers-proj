%{
#include "symtab.h"
#include "codegen.h"
#include "ast.h"
#include "tinylex.tab.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

extern int num_line;
extern bool declared;
extern int yylex();
extern void yyerror(char *s);

AST_Node *ast_head;
AST_Node *temp_return;
%}

/* Keywords */   
%token <value> IF 
%token <value> ELSE
%token <value> WHILE   
%token <value> CHAR
%token <value> INT
%token <value> FLOAT
%token <value> RETURN
%token <value> VOID

/* Pairs of tokens */
%token <value> LTPAR     /* ( */
%token <value> RTPAR     /* ) */
%token <value> LTBRACE   /* { */
%token <value> RTBRACE   /* } */    

/* Basic separators */
%token <value> SEMICOLON
%token <value> COMMA

/* Math operators */
%token <value> PLUS
%token <value> MINUS
%token <value> MULTIPLY
%token <value> DIVIDE

/* Equality and difference */
%token <value> ASSIGNMENT  /* = */
%token <value> EQUAL       /* == */
%token <value> NOT_EQUAL   /* != */

/* Logical operators */
%token GT      /* > */
%token LT      /* < */
%token GTE     /* >= */
%token LTE     /* <= */

%token PTR     /* & */

%token <symbol_item> IDENTIFIER
%token <value> CHAR_CONST
%token <value> INT_CONST
%token <value> FLOAT_CONST
%token <value> STRING_CONST

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
%type <node> constant
%type <data_type> type
%type <node> func_arglist
%type <node> func_call
%type <node> primary_exp
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
%type <data_type> return_type
%type <node> var_def
%type <symbol_item> func_def_identifier
%type <node> var_deflist
%type <node> func_st_list
%type <node> func_body
%type <param> func_param
%type <node> func_paramlist
%type <node> func_def
%type <node> func_deflist

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

program: func_deflist
    {
        ast_head = $1;
        #ifdef DEBUG
        ast_print_traversal(ast_head, 0);
        print_symtab();
        #endif
        generate_code();
    }
    ;

constant: INT_CONST { $$ = new_ast_const(INT_CONST_TYPE, $1); }
    | FLOAT_CONST { $$ = new_ast_const(FLOAT_CONST_TYPE, $1); }
    | STRING_CONST 
    { 
        $$ = new_ast_const(STRING_CONST_TYPE, $1);
    }
    | CHAR_CONST { $$ = new_ast_const(CHAR_CONST_TYPE, $1); }
    ;

type: INT { $$ = INT_TYPE; }
    | FLOAT { $$ = FLOAT_TYPE; }
    | CHAR { $$ = CHAR_TYPE; }
    ;

func_arglist: PTR IDENTIFIER
    {
        $2->passing = BY_REFER;
        AST_Node *temp = new_identifier_container($2);
        $$ = new_ast_function_call_params(NULL, 0, temp); /* TODO FIX THIS SHIT RELATED TO PARAMS*/
    }
    | exp
    {
        $$ = new_ast_function_call_params(NULL, 0, $1);
    }
    | exp COMMA func_arglist
    {
        AST_Function_Call_Params *temp = (AST_Function_Call_Params *) $3;
        $$ = new_ast_function_call_params(temp->params, temp->num_params, $1);
    }
    | PTR IDENTIFIER COMMA func_arglist
    {
        $2->passing = BY_REFER;
        AST_Function_Call_Params *temp = (AST_Function_Call_Params *) $4;
        AST_Node *temp2 = new_identifier_container($2);
        $$ = new_ast_function_call_params(temp->params, temp->num_params, temp2);
    }
    ;

func_call: IDENTIFIER LTPAR func_arglist RTPAR
    {
        AST_Function_Call_Params *temp = (AST_Function_Call_Params *) $3;
        $$ = new_ast_function_call($1, temp->params, temp->num_params);
    }
    | IDENTIFIER LTPAR RTPAR
    {
        $$ = new_ast_function_call($1, NULL, 0);
    }
    ;

primary_exp: constant { $$ = $1; }
    | IDENTIFIER
    {
        // Must use container b/c primary_exp is type node (AST_Node)
        $$ = new_identifier_container($1);
    }
    | func_call { $$ = $1; }
    | LTPAR exp RTPAR { $$ = $2; }
    ;

unary_exp: primary_exp { $$ = $1; }
    | PLUS unary_exp
    {
        if ($2->type == AST_UNARY) {
            // Just pass unary with unmodified sign if nested unary
            $$ = $2;
        } else {
            $$ = new_ast_unary(POSITIVE, $2);
        }
    }
    | MINUS unary_exp
    {
        if ($2->type == AST_UNARY) {
            // Only modify sign if nested unary
            AST_Unary *temp = (AST_Unary *) $2;
            if (temp->sign == NEGATIVE) {
                temp->sign = POSITIVE;  
            } else {
                temp->sign = NEGATIVE;
            }
            $$ = (AST_Node*) temp;
        } else {
            // Create new unary that holds primary_exp
            $$ = new_ast_unary(NEGATIVE, $2);
        }
    }
    ;

mult_exp: unary_exp { $$ = $1; }
    | mult_exp MULTIPLY unary_exp { $$ = new_ast_arith(MUL, $1, $3); }
    | mult_exp DIVIDE unary_exp { $$ = new_ast_arith(DIV, $1, $3); }
    ;

add_exp: mult_exp { $$ = $1; }
    | add_exp PLUS add_exp { $$ = new_ast_arith(ADD, $1, $3); }
    | add_exp MINUS add_exp { $$ = new_ast_arith(SUB, $1, $3); }
    ;

comp_exp: add_exp { $$ = $1; }
    | add_exp LT add_exp { $$ = new_ast_relat(LESS, $1, $3); }
    | add_exp LTE add_exp { $$ = new_ast_relat(LESS_EQUAL, $1, $3); }
    | add_exp GT add_exp { $$ = new_ast_relat(GREATER, $1, $3); }
    | add_exp GTE add_exp { $$ = new_ast_relat(GREATER_EQUAL, $1, $3); }
    ;

exp: comp_exp { $$ = $1; }
    | comp_exp EQUAL comp_exp { $$ = new_ast_equal(EQ, $1, $3); }
    | comp_exp NOT_EQUAL comp_exp { $$ = new_ast_equal(NOT_EQ, $1, $3); }
    ;

/* statements */

assign_st: IDENTIFIER ASSIGNMENT exp SEMICOLON
    {
        $$ = new_ast_assignment($1, $3);
    }
    ;

if_st: IF LTPAR exp RTPAR st %prec NO_ELSE
    {
        $$ = new_ast_if($3, $5, NULL);
    }
    | IF LTPAR exp RTPAR st ELSE st
    {
        $$ = new_ast_if($3, $5, $7);
    }
    ;

while_st: WHILE LTPAR exp RTPAR st
    {
        $$ = new_ast_while($3, $5);
    }
    ;

ret_st: RETURN SEMICOLON
    {
        $$ = new_ast_function_return(NULL);
    }
    | RETURN exp SEMICOLON {
        $$ = new_ast_function_return($2);
    }
    ;

st_list: /* epsilon */ { $$ = NULL; }
    | st st_list 
    {
        // Check not Null
        if ($1) {
            AST_Statements *temp = (AST_Statements *) $2;
            $$ = new_ast_statements(temp->statements, temp->num_statements, $1);
        }
    }
    ;

block_st: LTBRACE st_list RTBRACE { $$ = $2; }
    ;

empty_st: SEMICOLON { $$ = NULL; }
    ;

func_call_st: func_call SEMICOLON { $$ = $1; }
    ;

st: assign_st { $$ = $1; }
    | if_st { $$ = $1; }
    | while_st { $$ = $1; }
    | ret_st { $$ = $1; }
    | block_st { $$ = $1; }
    | empty_st { $$ = $1; }
    | func_call_st { $$ = $1; }
    ;

/* functions */

return_type: VOID { $$ = VOID_TYPE; }
    | type { $$ = $1; }
    ;

var_def: type { declared = true; } IDENTIFIER { declared = false; } ASSIGNMENT constant SEMICOLON
    {
        $3->token_type = $1;
        AST_Const *temp = (AST_Const *) $6;
        
        if ($1 == INT_TYPE) {
            $3->value.int_val = temp->val.int_value;
        } else if ($1 == CHAR_TYPE) {
            $3->value.char_val = temp->val.char_value;
        } else if ($1 == FLOAT_TYPE) {
            $3->value.float_val = temp->val.float_value;
        }
        $$ = new_ast_var_declaration($3, $3->token_type);
    }
    ;

var_deflist: /* epsilon */ { $$ = NULL; declared = false; }
    | var_def var_deflist
    {
        if ($2) {
            AST_Var_Declarations *temp = (AST_Var_Declarations *) $2;
            $$ = new_ast_var_declarations(temp->var_declarations, temp->num_vars, $1);
        } else {
            $$ = new_ast_var_declarations(NULL, 0, $1);
        }
    }
    ;

func_st_list: ret_st
    {
        temp_return = $1;
        $$ = NULL;
    }
    | st func_st_list
    {
        // If NULL, is a ret_st or empty_st. Do not add.
        if ($1) {
            AST_Statements *temp = (AST_Statements *) $2;
            new_ast_statements(temp->statements, temp->num_statements, $1);
        }
    }
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
        AST_Function_Declaration_Params *temp = (AST_Function_Declaration_Params*) $1;
        $$ = new_ast_function_declaration_params(temp->params, temp->num_params, $3);
    }
    ;

func_def_identifier: { declared = true; } IDENTIFIER
    {
        declared = false;
        $$ = $2;
    }
    ;

func_def: return_type func_def_identifier LTPAR func_paramlist RTPAR LTBRACE func_body RTBRACE
    {
        // func_body ($9) split b/c way grammar was designed, requires some sort
        // of node to be returned that stores var_def_list and func_st_list.
        // temp_return is a global used b/c return statement is buried in func_st_list,
        // easier to do this than to bubble up.
        // temp_return should be null for VOID function declarations (handled
        // in semantic analysis).
        $$ = new_ast_function_declaration($1, $2, $4, $7->left, $7->right, temp_return);
        ((AST_Function_Return*) temp_return)->return_type = $1;
        temp_return = NULL;
    }
    | return_type func_def_identifier LTPAR VOID RTPAR LTBRACE func_body RTBRACE
    {
        $$ = new_ast_function_declaration($1, $2, NULL, $7->left, $7->right, temp_return);
        ((AST_Function_Return*) temp_return)->return_type = $1;
        temp_return = NULL;
    }
    ;

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

    return 0;
}