#ifndef _codegen_h
#define _codegen_h

#include "ast.h"

#include <stdio.h>

int string_label_counter = 0;
char **string_consts;

void generate_code();
void generate_string_declarations(FILE *fp);
void generate_functions(FILE *fp);
void append_string_const(char *string_value);

#endif