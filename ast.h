#ifndef _ast_h
#define _ast_h

#include "symtab.h"

/* --- Node Types --- */
typedef enum NodeType {
    AST_NODE,   // Normal node with *left and *right

    // Declarations
    AST_VAR_DECLARATIONS,   // Variable declarations
    AST_VAR_DECLARATION,    // Variable declaration
    AST_CONST,              // Constant variable

    // Statements
    AST_STATEMENT,  // Statement
    AST_IF,         // If statement
    AST_WHILE,      // While statement
    AST_ASSIGNMENT, // Assignment statement
    AST_FUNC_CALL,  // Function call statement

    // Expressions
    ARITH_NODE,     // Arithmetic expression
    REL_NODE,       // Relational expression
    EQUAL_NODE,     // Equality expression

    // Functions
    AST_FUNC_BODY,         // Function body
    AST_FUNC_DECLARATIONS, // Function declarations
    AST_FUNC_DECLARATION,  // Function declaration
    AST_FUNC_DECLARATION_PARAMS,
    AST_RETURN,            // Function return
    AST_MAIN_RETURN,       // Main function return
} NodeType;

/* --- Value Union --- */
typedef union Value {
    int int_value;
    char char_value;
    float float_value;
    char *string_value;
} Value;

/* --- Operator Types --- */
typedef enum ArithOp {
    ADD, // + operator
    SUB, // - operator
    MUL, // * operator
    DIV, // / operator
} ArithOp;

typedef enum RelOp {
    GREATER,       // > operator
    LESS,          // < operator
    GREATER_EQUAL, // >= operator
    LESS_EQUAL,    // <= operator
} RelOp;

typedef enum EqualOp {
    EQ,     // == operator
    NOT_EQ, // != operator
} EqualOp;

/* --- Nodes --- */
typedef struct AST_Node {
    NodeType type;
    AST_Node *left;
    AST_Node *right;
} AST_Node;

typedef struct AST_Function_Declarations {
    NodeType type;
    AST_Node **func_declarations;
    int func_declaration_count;
} AST_Function_Declarations;

typedef struct AST_Function_Declaration {
    NodeType type;
    int return_type;
    Symbol *entry;
    AST_Node **params;
    AST_Node *body;
    AST_Node *return_node;
} AST_Function_Declaration;

typedef struct AST_Function_Declaration_Params {
    NodeType type;
    Symbol *entry;
    AST_Node *params;
    int num_params;
} AST_Function_Declaration_Params;

typedef struct AST_Function_Return {
    NodeType type;
    int return_type;
    AST_Node *return_value;
} AST_Function_Return;

typedef struct AST_Main_Return {
    NodeType type;
    AST_Node *return_value;
} AST_Main_Return;

/*
typedef struct AST_Const {
    NodeType type;
    int const_type;
    Value val;
} AST_Const;

typedef struct AST_Declarations {
    NodeType type;
    AST_Node **declarations;
    int declaration_count;
} AST_Declarations;

typedef struct AST_Decl {
    NodeType type;
    int var_type;
    Symbol **names;
    int names_count;
} AST_Decl;

typedef struct AST_Statements {
    NodeType type;
    AST_Node **statements;
    int statement_count;
} AST_Statements;

typedef struct AST_If {
    NodeType type;
    AST_Node *condition;
    AST_Node *if_branch;
    AST_Node *else_branch;
} AST_If;

typedef struct AST_While {
    NodeType type;
    AST_Node *condition;
    AST_Node *while_branch;
} AST_While;

typedef struct AST_Assign {
    NodeType type;
    Symbol *entry;
    AST_Node *assign_value;
} AST_Assign;

typedef struct AST_Arith {
    NodeType type;
    ArithOp op;
    AST_Node *left;
    AST_Node *right;
} AST_Arith;

typedef struct AST_Relat {
    NodeType type;
    RelOp op;
    AST_Node *left;
    AST_Node *right;
} AST_Relat;

typedef struct AST_Equal {
    NodeType type;
    EqualOp op;
    AST_Node *left;
    AST_Node *right;
} AST_Equal;

typedef struct AST_Function_Call {
    NodeType type;
    Symbol *entry;
    AST_Node **params;
    int num_params;
} AST_Function_Call;

typedef struct AST_Function_Call_Params {
    NodeType type;
    AST_Node **params;
    int num_params;
} AST_Function_Call_Params;
*/

#endif