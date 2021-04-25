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

void generate_functions(FILE *fp) {
    fprintf(fp, "\t.globl\tmain");

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