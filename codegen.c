#include "codegen.h"

#include <stdio.h>

void generate_code() {
    const char *file_name = "output.asm";
    FILE *fp = fopen(file_name, 'w');

    if (!fp) {
        printf("File %s cannot be opened! Exiting...", file_name);
        exit(1);
    }

    generate_string_declarations(fp);
    fprintf(fp, "\n");
    fprintf(fp, "\t.globl\tmain\n");
    generate_functions(fp);

    fclose(fp);
}

void generate_string_declarations(FILE *fp) {
    fprintf(fp, "\t.text\n");

    // String label number
    int string_num = 1;

    for (int i = 0; i < string_label_counter; i++) {
        char *string_const = string_consts[i];
        fprintf(fp, ".LC%d:\n", string_num);
        fprintf(fp, "\t.string:\t\"%s\"\n", string_const);
    }
}

void append_string_const(char *string_value) {
    if (!string_consts) {
        // First 
        string_consts = (char**) malloc(sizeof(char*));
        string_consts[0] = string_value;
        string_label_counter++;
    } else {
        string_consts = (char**) remalloc(string_consts, (string_label_counter + 1) * sizeof(char*));
        string_consts[string_label_counter] = string_value;
        string_label_counter++;
    }
}

void generate_func_deflist(FILE *fp, AST_Function_Declarations *node) {
    for (int i = 0; i < node->func_declaration_count; i++) {
        generate_func_def(fp, node->func_declarations[i]);
        fprintf(fp, "\n");
    }
}

void generate_func_def(FILE *fp, AST_Function_Declaration *node) {
    fprintf(fp, "pushl\t%%ebp\n");
    fprintf(fp, "movl\t%%esp, %%ebp\n");
    AST_Function_Declaration_Params *params = (AST_Function_Declaration_Params*)node->params;
    AST_Var_Declarations *var_decs = (AST_Var_Declarations*)node->var_declarations;
    int offset = 0; 
    if (params) {
        offset += params->num_params;
    }
    if (var_decs) {
        offset += var_decs->num_vars; 
    }
    offset *= 4; 
    if (offset != 0) {
        fprintf(fp, "subl\t$%d, %%esp\n", offset);
    }
}

void generate_func_params(FILE *fp, AST_Function_Declaration_Params *node) {
    if (params) {
        fprintf(fp, "movel\t%%edi, -4(ebp)");
        fprintf(fp, "movel\t%%edi, -8(ebp)");
    }
}

//var declarations maybe save it? 

