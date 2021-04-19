#ifndef symtab
#define symtab

#include <stdbool.h>
#include <stdlib.h>

#define MAX_IDENTIFIER_LENGTH 48

/* Token Types */
#define UNDEF 0
#define INT_TYPE 1
#define FLOAT_TYPE 2
#define CHAR_TYPE 3
#define STR_TYPE 4
#define FUNCTION_TYPE 5
#define LOGIC_TYPE 6

/* Function Parameter Passing */
#define BY_VALUE 0
#define BY_REFER 1

static Symbol *sym_table = NULL;
static scope_t curr_scope = 0;
// Parser sets declared in rule, lexer calls symtab append,
// symtab append checks if able to add w/ declared bool.
static bool declared = false;

typedef unsigned int scope_t;

typedef struct Param {
    int param_type;
    char param_name[MAX_IDENTIFIER_LENGTH];
    union value {
        int int_val;
        float float_val;
        char char_val;
        char *str_val;
    } value;

    // Value (0) or Reference (1)
    int passing;
} Param;

typedef struct RefList {
    int line_num;
    struct RefList *next;
} Reflist;

typedef struct Symbol {
    char *name;
    int name_size;

    scope_t scope;
    bool declared;

    int token_type;
    union value {
        int int_val;
        float float_val;
        char char_val;
        char *str_val;
    } value;

    // For function symbols
    int return_type;

    // Function params
    Param *parameters;
    int num_params;

    #ifdef DEBUG
    Reflist *lines;
    int count;
    #endif

    // Next symbol in Symbol list
    Symbol *next;
} Symbol;

void append_sym(char *name, int len, int type);
Symbol *lookup_sym(char *name);
Symbol *lookup_sym_scoped(char *name, scope_t scope);
bool *is_sym_declared_scoped(char *name, scope_t scope);
void inc_scope();
void hide_scope();
void print_symtab();

#endif