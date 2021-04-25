#ifndef _symtab_h
#define _symtab_h

#include <stdbool.h>
#include <stdlib.h>

/* Token Types */
#define UNDEF 0
#define INT_TYPE 1
#define FLOAT_TYPE 2
#define CHAR_TYPE 3
#define STR_TYPE 4
#define FUNCTION_TYPE 5
#define LOGIC_TYPE 6
#define VOID_TYPE

/* Const Types */
#define INT_CONST_TYPE 0
#define FLOAT_CONST_TYPE 1
#define CHAR_CONST_TYPE 2
#define STRING_CONST_TYPE 3

/* Function Parameter Passing */
#define BY_VALUE 0
#define BY_REFER 1

static struct Symbol *sym_table = NULL;

typedef struct Param {
    int param_type;
    char *param_name;
    union param_value {
        int int_val;
        float float_val;
        char char_val;
        char *str_val;
    } param_value;

    // Value (0) or Reference (1)
    int passing;
} Param;

#ifdef DEBUG
typedef struct RefList {
    int line_num;
    struct RefList *next;
} RefList;
#endif

typedef struct Symbol {
    char *name;
    int token_type;
    union value {
        int int_val;
        float float_val;
        char char_val;
        char *str_val;
    } value;

    // For function symbols
    int return_type;
    Param *parameters;
    int num_params;

    // For function call symbol

    #ifdef DEBUG
    RefList *lines;
    int count;
    #endif

    // Next symbol in Symbol list
    struct Symbol *next;
} Symbol;

void append_sym(char *name, int type);
Symbol *lookup_sym(char *name);
Param def_param(int param_type, char *param_name, int passing);

#ifdef DEBUG
void print_symtab();
#endif

#endif