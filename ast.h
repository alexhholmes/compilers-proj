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
    AST_STATEMENTS,
    AST_IF,         // If statement
    AST_WHILE,      // While statement
    AST_ASSIGNMENT, // Assignment statement
    AST_FUNC_CALL_PARAMS,
    AST_FUNC_CALL,  // Function call statement

    // Expressions
    AST_UNARY,
    AST_ARITH,
    AST_RELAT,
    AST_EQUAL,

    // Functions
    AST_FUNC_BODY,         // Function body
    AST_FUNC_DECLARATIONS, // Function declarations
    AST_FUNC_DECLARATION,  // Function declaration
    AST_FUNC_DECLARATION_PARAMS,
    AST_FUNC_RETURN,
    AST_MAIN_RETURN,       // Main function return

    AST_IDENTIFIER_CONTAINER,
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

typedef enum Sign {
    POSITIVE,
    NEGATIVE,
} Sign;

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
    AST_Node *params;
    AST_Node *var_declarations;
    AST_Node *statements;
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

typedef struct AST_Const {
    NodeType type;
    int const_type;
    Value val;
} AST_Const;
typedef struct AST_Assignment {
    NodeType type;
    Symbol *entry;
    AST_Node *assign_value;
} AST_Assignment;
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

typedef struct AST_Function_Call {
    NodeType type;
    Symbol *entry;
    AST_Node *params;
    int num_params;
} AST_Function_Call;

typedef struct AST_Function_Call_Params {
    NodeType type;
    Param *params;
    int num_params;
    // A call param can be a expression, so we do not use Param
    // like AST_Function_Declaration_Params.
} AST_Function_Call_Params;

typedef struct AST_Statements {
    NodeType type;
    AST_Node **statements;
    int num_statements;
} AST_Statements;

typedef struct AST_Var_Declarations {
    NodeType type;
    AST_Node **var_declarations;
    int num_vars;
} AST_Var_Declarations;

typedef struct AST_Var_Declaration {
    NodeType type;
    int data_type;
    Symbol *entry;
    // Value stored in symbol table entry
} AST_Var_Declaration;

typedef struct AST_Unary {
    NodeType type;
    Sign sign;
    NodeType *expression;
} AST_Unary;

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

typedef struct AST_Identifier_Container {
    NodeType type;
    Symbol *entry;
} AST_Identifier_Container;

#endif