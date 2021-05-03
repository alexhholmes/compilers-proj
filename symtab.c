#include "symtab.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern int num_line;

bool declared = false;

/*
 * Appends a new symbol to the front of the symbol table.
 * Checks if variable has already been declared in the scope.
 */
void append_sym(char *name, int type) {
    if (declared) {
        // Declared symbol must check if it's already been declared
        // in the current scope.
        // TODO Have this be done by semantic analyzer so error is handled there?
        if (!lookup_sym(name)) {
            Symbol *new_sym = (Symbol *) malloc(sizeof(Symbol));
            new_sym->name = name;
            new_sym->token_type = type;
            new_sym->next = sym_table;
            sym_table = new_sym;

            #ifdef DEBUG
            printf("New symbol: %s, ln %d\n", name, num_line);

            new_sym->count = 0;
            new_sym->lines = (RefList *) malloc(sizeof(RefList));
            new_sym->lines->line_num = num_line;
            new_sym->lines->next = NULL;
            #endif
        }
    } 

    // Do nothing for symbols that are not newly declared, the AST
    // will use the lookup func to get the symbol by name and then
    // update its value.
}

/*
 * Returns a reference to the symbol of the given name, NULL if
 * symbol does not exist. This returns the most locally scoped
 * symbol.
 */
Symbol *lookup_sym(char *name) {
    Symbol *sym = sym_table;
    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
        sym = sym->next;
    }
    return NULL;
}

#ifdef DEBUG
/*
 * Prints the symbol table with indentation representing scope.
 */
void print_symtab() {
    Symbol *sym = sym_table;
    while (sym != NULL) {
        printf("%s\t\t%d\t", sym->name, sym->token_type);
        printf("\n");
        sym = sym->next;
    }
}
#endif

Param def_param(int param_type, char *param_name, int passing) {
    Param param;

    param.param_name = strdup(param_name);
    param.param_type = param_type;
    param.passing = passing;
    
    return param;
}