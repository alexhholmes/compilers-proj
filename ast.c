#include "ast.h"

#include <stdio.h>
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

AST_Node *new_ast_function_declarations(AST_Node **func_declarations, int func_declaration_count, AST_Node *func_declaration) {
    AST_Function_Declarations *node = malloc(sizeof(AST_Function_Declarations));

    node->type = AST_FUNC_DECLARATIONS;
    
    if (!func_declarations) {
        // First declaration
        // Array of AST_Node pointers (AST_Nodes are later typecast to proper node type)
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

AST_Node *new_ast_function_declaration(int return_type, Symbol *entry, AST_Node *params, AST_Node *var_declarations, AST_Node *statements, AST_Node *return_node) {
    AST_Function_Declaration *node = malloc(sizeof(AST_Function_Declaration));

    node->type = AST_FUNC_DECLARATION;
    node->return_type = return_type;
    node->entry = entry;
    node->params = params;
    node->var_declarations = var_declarations;
    node->statements = statements;
    node->return_node = return_node;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_function_return(AST_Node *return_value) {
    AST_Function_Return *node = malloc(sizeof(AST_Function_Return));

    node->type = AST_FUNC_RETURN;
    // node->return_type is set in tinylex.y:func_def
    node->return_value = return_value;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_main_return(AST_Node *return_value) {
    AST_Main_Return *node = malloc(sizeof(AST_MAIN_RETURN));

    node->type = AST_MAIN_RETURN;
    node->return_value = return_value;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_const(int const_type, Value val) {
    AST_Const *node = malloc(sizeof(AST_Const));

    node->type = AST_CONST;
    node->val = val;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_assignment(Symbol *entry, AST_Node *assign_value) {
    AST_Assignment *node = malloc(sizeof(AST_Assignment));

    node->type = AST_ASSIGNMENT;
    node->entry = entry;
    node->assign_value = assign_value;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_if(AST_Node *condition, AST_Node *if_branch, AST_Node *else_branch) {
    AST_If *node = malloc(sizeof(AST_If));

    node->type = AST_IF;
    node->condition = condition;
    node->if_branch = if_branch;
    node->else_branch = else_branch;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_while(AST_Node *condition, AST_Node *while_branch) {
    AST_While *node = malloc(sizeof(AST_While));

    node->type = AST_WHILE;
    node->condition = condition;
    node->while_branch = while_branch;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_function_call(Symbol *entry, AST_Node *params, int num_params) {
    AST_Function_Call *node = malloc(sizeof(AST_Function_Call));

    node->type = AST_FUNC_CALL;
    node->entry = entry;
    node->params = params;
    node->num_params = num_params;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_function_call_params(AST_Node **params, int num_params, AST_Node *param) {
    AST_Function_Call_Params *node = malloc(sizeof(AST_Function_Call_Params));

    node->type = AST_FUNC_CALL_PARAMS;
    
    if (!params) {
        // First parameter
        params = (AST_Node**) malloc(sizeof(AST_Node*));
        params[0] = param;
        num_params = 1;
    } else {
        // Append new parameter
        params = (AST_Node**) realloc(params, (num_params + 1) * sizeof(AST_Node*));
        num_params++;
    }

    node->params = params;
    node->num_params = num_params;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_statements(AST_Node **statements, int num_statements, AST_Node *statement) {
    AST_Statements *node = malloc(sizeof(AST_Statements));

    node->type = AST_STATEMENTS;
    
    if (!statements) {
        // First statement
        statements = (AST_Node **) malloc(sizeof(AST_Node*));
        statements[0] = statement;
        num_statements = 1;
    } else {
        // Append new statement
        statements = (AST_Node**) realloc(statements, (num_statements + 1) * sizeof(AST_Node*));
        statements[num_statements] = statement;
        num_statements++;
    }

    node->statements = statements;
    node->num_statements = num_statements;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_var_declarations(AST_Node **var_declarations, int num_vars, AST_Node *var_declaration) {
    AST_Var_Declarations *node = malloc(sizeof(AST_Var_Declarations));

    node->type = AST_VAR_DECLARATIONS;

    if (!var_declarations) {
        // First variable declaration
        var_declarations = (AST_Node **) malloc(sizeof(AST_Node*));
        var_declarations[0] = var_declaration;
        num_vars = 1;
    } else {
        // Append new variable declaration
        var_declarations = (AST_Node**) realloc(var_declarations, (num_vars + 1) * sizeof(AST_Node*));
        var_declarations[num_vars] = var_declaration;
        num_vars++;
    }

    node->var_declarations = var_declarations;
    node->num_vars = num_vars;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_var_declaration(Symbol *entry, int data_type) {
    AST_Var_Declaration *node = malloc(sizeof(AST_Var_Declaration));

    node->type = AST_VAR_DECLARATION;
    node->data_type = data_type;
    node->entry = entry;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_arith(ArithOp op, AST_Node *left, AST_Node *right) {
    AST_Arith *node = malloc(sizeof(AST_Arith));

    node->type = AST_ARITH;
    node->op = op;
    node->left = left;
    node->right = right;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_relat(RelatOp op, AST_Node *left, AST_Node *right) {
    AST_Relat *node = malloc(sizeof(AST_Relat));

    node->type = AST_RELAT;
    node->op = op;
    node->left = left;
    node->right = right;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_equal(EqualOp op, AST_Node *left, AST_Node *right) {
    AST_Equal *node = malloc(sizeof(AST_Equal));

    node->type = AST_EQUAL;
    node->op = op;
    node->left = left;
    node->right = right;

    return (struct AST_Node *) node;
}

AST_Node *new_identifier_container(Symbol *entry) {
    AST_Identifier_Container *node = malloc(sizeof(AST_Identifier_Container));

    node->type = AST_IDENTIFIER_CONTAINER;
    node->entry = entry;

    return (struct AST_Node *) node;
}

AST_Node *new_ast_unary(Sign sign, AST_Node *expression) {
    AST_Unary *node = malloc(sizeof(AST_Unary));

    node->type = AST_UNARY;
    node->sign = sign;
    node->expression = expression;

    return (struct AST_Node *) node;
}

void ast_print_traversal(AST_Node *node, int indention) {
    switch (node->type) {
        case AST_NODE:
            {
                ast_indented_println("AST_NODE", indention);
                if (node->left) ast_print_traversal(node->left, indention + 1);
                if (node->right) ast_print_traversal(node->right, indention + 1);
            }

            break;

        case AST_FUNC_DECLARATION_PARAMS:
            {
                AST_Function_Declaration_Params *temp = (AST_Function_Declaration_Params*) node;
                
                ast_indented_print("AST_FUNC_DECLARATION_PARAMS: ", indention);

                // Print space-separated params of this function declaration
                Param *params = temp->params;
                for (int i = 0; i < temp->num_params; i++) {
                    Param param = params[i];
                    printf("%s ", param.entry->name);
                }
                printf("\n");
            }

            break;

        case AST_FUNC_DECLARATIONS:
            {
                AST_Function_Declarations *temp = (AST_Function_Declarations*) node;

                ast_indented_println("AST_FUNC_DECLARATIONS", indention);

                // Traverse the individual function declarations
                for (int i = 0; i < temp->func_declaration_count; i++) {
                    ast_print_traversal(temp->func_declarations[i], indention + 1);
                }
            }

            break;

        case AST_FUNC_DECLARATION:
            {
                AST_Function_Declaration *temp = (AST_Function_Declaration*) node;

                ast_indented_println("AST_FUNC_DECLARATION", indention);
                if (temp->params) ast_print_traversal(temp->params, indention + 1);
                if (temp->var_declarations) ast_print_traversal(temp->var_declarations, indention + 1);
                if (temp->statements) ast_print_traversal(temp->statements, indention + 1);
                if (temp->return_node) ast_print_traversal(temp->return_node, indention + 1);
            }

            break;

        case AST_FUNC_RETURN:
            {
                AST_Function_Return *temp = (AST_Function_Return*) node;

                ast_indented_println("AST_FUNC_RETURN", indention);
                if (temp->return_value) ast_print_traversal(temp->return_value, indention + 1);
            }

            break;

        case AST_MAIN_RETURN:
            {
                AST_Main_Return *temp = (AST_Main_Return*) node;

                ast_indented_println("AST_MAIN_RETURN", indention);
                ast_print_traversal(temp->return_value, indention + 1);
            }

            break;

        case AST_ASSIGNMENT:
            {
                AST_Assignment *temp = (AST_Assignment*) node;

                ast_indented_println("AST_ASSIGNMENT", indention);
                ast_print_traversal(temp->assign_value, indention + 1);
            }

            break;

        case AST_IF:
            {
                AST_If *temp = (AST_If*) node;

                ast_indented_println("AST_IF", indention);
                ast_print_traversal(temp->condition, indention + 1);
                ast_print_traversal(temp->if_branch, indention + 1);
                if (temp->else_branch) ast_print_traversal(temp->else_branch, indention + 1);
            }

            break;

        case AST_WHILE:
            {
                AST_While *temp = (AST_While*) node;

                ast_indented_println("AST_WHILE", indention);
                ast_print_traversal(temp->condition, indention + 1);
                ast_print_traversal(temp->while_branch, indention + 1);
            }

            break;

        case AST_FUNC_CALL:
            {
                AST_Function_Call *temp = (AST_Function_Call*) node;

                ast_indented_print("AST_FUNC_CALL", indention);
                if (temp->params) ast_print_traversal(temp->params, indention + 1);
            }

            break;

        case AST_FUNC_CALL_PARAMS:
            {
                AST_Function_Call_Params *temp = (AST_Function_Call_Params*) node;

                ast_indented_println("AST_FUNC_CALL_PARAMS", indention);
                for (int i = 0; i < temp->num_params; i++) {
                    ast_print_traversal(temp->params[i], indention + 1);
                }
            }

            break;

        case AST_STATEMENTS:
            {
                AST_Statements *temp = (AST_Statements*) node;

                ast_indented_println("AST_STATEMENTS", indention);
                for (int i = 0; i < temp->num_statements; i++) {
                    ast_print_traversal(temp->statements[i], indention + 1);
                }
            }

            break;

        case AST_VAR_DECLARATIONS:
            {
                AST_Var_Declarations *temp = (AST_Var_Declarations*) node;

                ast_indented_println("AST_VAR_DECLARATIONS", indention);
                for (int i = 0; i < temp->num_vars; i++) {
                    ast_print_traversal(temp->var_declarations[i], indention + 1);
                }
            }

            break;

        case AST_VAR_DECLARATION:
            {
                AST_Var_Declaration *temp = (AST_Var_Declaration*) node;

                ast_indented_println("AST_VAR_DECLARATION", indention);
            }

            break;

        case AST_ARITH:
            {
                AST_Arith *temp = (AST_Arith*) node;

                ast_indented_println("AST_ARITH", indention);
                ast_print_traversal(temp->left, indention + 1);
                ast_print_traversal(temp->right, indention + 1);
            }

            break;

        case AST_RELAT:
            {
                AST_Relat *temp = (AST_Relat*) node;

                ast_indented_println("AST_RELAT", indention);
                ast_print_traversal(temp->left, indention + 1);
                ast_print_traversal(temp->right, indention + 1);
            }

            break;

        case AST_EQUAL:
            {
                AST_Equal *temp = (AST_Equal*) node;

                ast_indented_println("AST_EQUAL", indention);
                ast_print_traversal(temp->left, indention + 1);
                ast_print_traversal(temp->right, indention + 1);
            }

            break;

        case AST_IDENTIFIER_CONTAINER:
            ast_indented_println("AST_IDENTIFIER_CONTAINER", indention);

            break;

        case AST_UNARY:
            {
                AST_Unary *temp = (AST_Unary*) node;

                ast_indented_println("AST_UNARY", indention);
                ast_print_traversal(temp->expression, indention + 1);
            }

            break;

        case AST_CONST:
            {
                AST_Const *temp = (AST_Const*) node;

                ast_indented_println("CONST", indention);
            }
            break;

        default:
            printf("Unknown AST type!");
            exit(1);
    }
}

void ast_indented_println(char *node_type, int indention) {
    ast_indented_print(node_type, indention);
    printf("\n");
}

void ast_indented_print(char *node_type, int indention) {
    for (int i = 0; i < indention; i++) {
        putchar('\t');
    }
    printf("%s", node_type);
}