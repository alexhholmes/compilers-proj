#include "ast.h"

#include <string.h>

AST_Node *new_ast_node(NodeType type, AST_Node *left, AST_Node *right) {
    AST_Node *node = malloc(sizeof(AST_Node));

    node->type = type;
    node->left = left;
    node->right = right;

    return node;
}

AST_Node *new_ast_function_declaration_params(Param *parameters, int num_of_params, Param param) {
    AST_Function_Declaration_Params *node = malloc(sizeof(AST_Function_Declaration_Params));

    node->type = AST_FUNC_DECLARATION_PARAMS;

    if (!parameters) {
        // First parameter declaration
        parameters = (Param*) malloc(sizeof(Param));
        parameters[0] = param;
        num_of_params = 1;
    } else {
        // Append new parameter declaration
        parameters = (Param*) realloc(parameters, (num_of_params + 1) * sizeof(Param));
        parameters[num_of_params] = param;
        num_of_params++;
    }

    node->params = parameters;
    node->num_params = num_of_params;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_function_declarations(AST_Node **func_declarations, int func_declaration_count, AST_Function_Declaration *func_declaration) {
    AST_Function_Declarations *node = malloc(sizeof(AST_Function_Declarations));

    node->type = AST_FUNC_DECLARATIONS;
    
    if (!func_declarations) {
        // First declaration
        func_declarations = (AST_Node**) malloc(sizeof(AST_Node*));
        func_declarations[0] = func_declaration;
        func_declaration_count = 1;
    } else {
        // Add new declaration
        func_declarations = (AST_Node**) realloc(func_declarations, (func_declaration_count + 1) * sizeof(AST_Node*));
        func_declarations[func_declaration_count] = func_declaration;
        func_declaration_count++;
    }

    node->func_declarations = func_declarations;
    node->func_declaration_count = func_declaration_count;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_function_declaration(int return_type, Symbol *entry, AST_Node *params, AST_Node *body, AST_Node *return_node) {
    AST_Function_Declaration *node = malloc(sizeof(AST_Function_Declaration));

    node->type = AST_FUNC_DECLARATION;
    node->return_type = return_type;
    node->entry = entry;
    node->params = params;
    node->body = body;
    node->return_node = return_node;

    return (struct AST_Node *) node;
}

/*
AST_Node *new_ast_const(int const_type, Value value) {
    AST_Const *node = malloc(sizeof(AST_Const));

    node->type = CONST_NODE;
    node->const_type = const_type;
    node->val = value;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_declarations(AST_Node **declarations, int declaration_count, AST_Node *declaration) {
    AST_Declarations *node = malloc(sizeof(AST_Declarations));

    node->type = DECLARATIONS_NODE;

    if (!declarations) {
        // First declaration
        declarations = (AST_Node**) malloc(sizeof(AST_Node*));
        declarations[0] = declaration;
        declaration_count = 1;
    } else {
        // Add new declaration
        declarations = (AST_Node**) realloc(declarations, (declaration_count + 1) * sizeof(AST_Node*)));
        declarations[declaration_count] = declaration;
        declaration_count++;
    }

    node->declarations = declaration;
    node->declaration_count = declaration_count;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_decl(int var_type, Symbol **names, int names_count) {
    AST_Decl *node = malloc(sizeof(AST_Decl));

    node->type = DECL_NODE;
    node->var_type = var_type;
    node->names = names;
    node->names_count = names_count;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_statements(AST_Node **statements, int statement_count, AST_Node *statement) {
    AST_Statements *node = malloc(sizeof(AST_Statements));

    node->type = STATEMENTS_NODE;

    if (!statements) {
        // First statement
        statements = (AST_Node**) malloc(sizeof(AST_Node*));
        statements[0] = statements;
        statement_count = 1;
    } else {
        // Add new statement
        statements = (AST_Node**) realloc(statements, (statement_count + 1) * sizeof(AST_Node*));
        statements[statement_count] = statement;
        statement_count++;
    }

    node->statements = statements;
    node->statements_count = statement_count;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_if(AST_Node *condition, AST_Node *if_branch, AST_Node else_branch) {
    AST_If *node = malloc(sizeof(AST_If));

    node->type = IF_NODE;
    node->condition = condition;
    node->if_branch = if_branch;
    node->else_branch = else_branch;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_while(AST_Node *condition, AST_Node *while_branch) {
    AST_While *node = malloc(sizeof(AST_While));

    node->type = WHILE_NODE;
    node->condition = condition;
    node->while_branch = while_branch;

    return (struct AST_Node *) node;
}
*/