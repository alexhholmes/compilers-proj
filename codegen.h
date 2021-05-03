#ifndef _codegen_h
#define _codegen_h

#include "ast.h"

#include <stdio.h>

char **string_consts;

void generate_code();
void generate_string_declarations(FILE *fp);
void append_string_const(char *string_value);
void generate_func_deflist(FILE *fp, AST_Function_Declarations *node);
void generate_func_def(FILE *fp, AST_Function_Declaration *node);
int generate_func_params(FILE *fp, AST_Function_Declaration_Params *node);
void generate_var_declarations(FILE *fp, AST_Var_Declarations *node, int offset);
void generate_statements(FILE *fp, AST_Statements *node);
void generate_statement(FILE *fp, AST_Node *statement);
void generate_assignment(FILE *fp, AST_Assignment *node);
void generate_if(FILE *fp, AST_If *node);
void generate_while(FILE *fp, AST_While *node);
void generate_function_return(FILE *fp, AST_Function_Return* node);
void generate_function_call(FILE *fp, AST_Function_Call *node);
void generate_const(FILE *fp, AST_Const *node);
void generate_arith(FILE *fp, AST_Arith *node);
void generate_equal(FILE *fp, AST_Equal *node);
void generate_exp(FILE *fp, AST_Node *node);
int generate_exp_cmp(FILE *fp, AST_Node *node);
int get_label_num();
void fail(FILE *fp, char *err_message);


#endif