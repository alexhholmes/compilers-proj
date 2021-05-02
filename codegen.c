#include "codegen.h"
#include "symtab.h"
#include "ast.h"

#include <stdio.h>

#define EAX "%%eax"
#define EBX "%%ebx"
#define ECX "%%ecx"
#define EDX "%%edx"
#define EDI "%%edi"
#define ESI "%%esi"
#define EBP "%%ebp"
#define ESP "%%esp"
#define IMM(num) "$"STRINGIFY(num)
#define MEM(offset, reg) STRINGIFY(offset)"("reg")"

#define PUSH(reg) fprintf(fp, "pushl\t"reg"\n")
#define POP(reg) fprintf(fp, "popl\t"reg"\n")
#define CALL(func) fprintf(fp, "call\t%s\n", func)
#define RET fprintf(fp, "ret\n")
#define MOV(arg1, arg2) fprintf(fp, "movl\t"arg1", "arg2"\n")
#define LEA(arg1, arg2) fprintf(fp, "leal\t"arg1", "arg2"\n")
#define ADD(arg1, arg2) fprintf(fp, "addl\t"arg1", "arg2"\n")
#define SUB(arg1, arg2) fprintf(fp, "subl\t"arg1", "arg2"\n")

#define STRINGIFY(x) #x

/* Dear Professor or TA, if you see this, then please know how proud I am of these
macros I made. They are my babies. My beautiful creations. They are my everything.
I am nothing without them. My brain grew 3x in size when I made them from scratch.
Bask in their glory for their words are that of the holy scripture. */

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
    // Store %ebp and set it to %esp value
    PUSH(EBP);
    MOV(ESP, EBP);

    AST_Function_Declaration_Params *params = (AST_Function_Declaration_Params*)node->params;
    AST_Var_Declarations *var_decs = (AST_Var_Declarations*)node->var_declarations;

    // Calculate stack pointer offset
    int offset = 0; 
    if (params) offset += params->num_params;
    if (var_decs) offset += var_decs->num_vars; 
    offset *= 4; 

    // Only offset stack pointer if we have declared variables to store on stack
    if (offset != 0) {
        SUB(IMM(offset), ESP);
    }

    // Move params and local vars to stack
    int curr_var_offset = generate_func_params(fp, params);
    generate_var_declarations(fp, var_decs, curr_var_offset);

    // Generate statements
    AST_Statements *statements = (AST_Statements*) node->statements;
    generate_statements(fp, statements);
}

int generate_func_params(FILE *fp, AST_Function_Declaration_Params *node) {
    int offset = -4;

    if (node->params) {
        // Loop through all function parameters to move from registers to stack
        for (int i = 0; i < node->num_params; i++) {
            if (i > 1) {
                printf("Lol you thought we'd actually implement this to work with more than 2 params. You wrong.\n");
                exit(1);
            }

            Param param = node->params[i];
            switch (param.param_type) {
                case INT_TYPE:
                    {
                        if (i == 0) {
                            MOV(EDI, MEM(offset, EBP));
                            offset -= 4;
                        }
                        if (i == 1) {
                            MOV(ESI, MEM(offset, EBP));
                            offset -= 4;
                        }
                    }
                    break;
                case FLOAT_TYPE:
                    {
                        // TODO
                    }
                    break;
                case CHAR_TYPE:
                    {
                        if (i == 0) {
                            MOV(EDI, MEM(offset, EBP));
                            offset -= 4;
                        }
                        if (i == 1) {
                            MOV(ESI, MEM(offset, EBP));
                            offset -= 4;
                        }
                    }
                    break;
                default:
                    printf("codegen.c:generate_func_params() unknown type\n");
                    exit(1);
            }
        }
    }
    return offset;
}

void generate_var_declarations(FILE *fp, AST_Var_Declarations *node, int offset) {
    if (node->var_declarations) {
        // Loop through all variable declarations and move constant values to stack
        for (int i = 0; i < node->num_vars; i++) {
            AST_Var_Declaration *var = (AST_Var_Declaration*) node->var_declarations[i];
            switch (var->data_type) {
                case INT_TYPE:
                    {
                        Symbol *entry = var->entry;
                        entry->offset = offset;
                        MOV(IMM(entry->value.int_val), MEM(offset, EBP));
                        offset -= 4;
                    }
                    break;
                case FLOAT_TYPE:
                    {
                        // TODO
                    }
                    break;
                case CHAR_TYPE:
                    {
                        Symbol *entry = var->entry;
                        entry->offset = offset;
                        MOV(IMM(entry->value.int_val), MEM(offset, EBP));
                        offset -= 4;
                    }
                    break;
                default:
                    printf("codegen.c:generate_var_declarations() unknown type\n");
                    exit(1);
            }
        }
    }
}

void generate_statements(FILE *fp, AST_Statements *node) {
    for (int i = 0; i < node->num_statements; i++) {
        AST_Node *statement = node->statements[i];
        switch (statement->type) {
            case AST_ASSIGNMENT:
                {
                    AST_Assignment *temp = (AST_Assignment*) statement;
                    generate_assignment(fp, temp);
                }
                break;
            case AST_IF:
                {
                    
                }
                break;

            case AST_WHILE:
                {

                }
                break;

            case AST_FUNC_RETURN:
                {
                    AST_Function_Return *temp = (AST_Function_Return*) statement;
                    generate_function_return(fp, temp);
                }
                break;

            case AST_FUNC_CALL:
                {
                    AST_Function_Call *temp = (AST_Function_Call*) statement;
                    generate_function_call(fp, temp);
                }
                break;

            case AST_STATEMENTS:
                {
                    generate_statements(fp, statement);
                }
                break;

            default:
                    printf("codegen.c:generate_statements() unknown type\n");
                    exit(1);
        }
    }
}

void generate_assignment(FILE *fp, AST_Assignment* node) {
    generate_exp(fp, node->assign_value);
    int offset = node->entry->offset;
    MOV(EBX, MEM(offset, EBP));
}

void generate_function_return(FILE *fp, AST_Function_Return* node) {
    switch (node->return_type) {
        case INT_TYPE:
            {
                generate_exp(fp, node->return_value);
                MOV(EBX, EAX);
            }
            break;
        case FLOAT_TYPE:
            {
                // TODO
            }
            break;
        case CHAR_TYPE:
            {
                generate_exp(fp, node->return_value);
                MOV(EBX, EAX);
            }
            break;
        case VOID_TYPE:
            // Do nothing
            break;
        default:
            printf("codegen.c:generate_var_declarations() unknown type\n");
            exit(1);
    }

    POP(EBP);
    RET;
}

void generate_function_call(FILE *fp, AST_Function_Call *node) {
    char *func_name = node->entry->name;

    // Load params into registers
    for (int i = 0; i < node->num_params; i++) {
        if (i > 1) {
            printf("Lol you thought we'd actually implement this to work with more than 2 function args. You wrong.\n");
            exit(1);
        }

        AST_Node *param = node->params[i];
        switch (param->type) {
            case AST_IDENTIFIER_CONTAINER:
                {
                    AST_Identifier_Container *identifier = (AST_Identifier_Container*) param;
                    
                    // Put address into arg register
                    int offset = identifier->entry->offset;
                    if (i == 0) {
                        LEA(MEM(offset, EBP), EDI);
                    } else {
                        LEA(MEM(offset, EBP), ESI);
                    }
                }
                break;
            defaut: // Expression
                {
                    // Move expression result from %ebx to arg register
                    generate_exp(fp, param);
                    
                    if (i == 0) {
                        MOV(EBX, EDI);
                    } else {
                        MOV(EBX, ESI);
                    }
                }
            
        }
    }

    CALL(func_name);
}

/*
 * Result will ALWAYS be in %ebx.
 */
void generate_exp(FILE *fp, AST_Node* node) {
}