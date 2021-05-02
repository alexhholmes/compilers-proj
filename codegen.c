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
#define EIP "%%esp"
#define IMM(num) "$"STRINGIFY(num)
#define MEM(offset, reg) STRINGIFY(offset)"("reg")"
#define STR(label_num) ".LC"STRINGIFY(label_num)"("EIP")"

#define LABEL(name) fprintf(fp, "%s:\n", name)
#define PUSH(reg) fprintf(fp, "\tpushl\t"reg"\n")
#define POP(reg) fprintf(fp, "\tpopl\t"reg"\n")
#define CALL(func) fprintf(fp, "\tcall\t%s\n", func)
#define RET fprintf(fp, "\tret\n")
#define MOV(src, dest) fprintf(fp, "\tmovl\t"src", "dest"\n")
#define LEA(arg1, arg2) fprintf(fp, "\tleal\t"arg1", "arg2"\n")
#define ASM_ADD(arg1, arg2) fprintf(fp, "\taddl\t"arg1", "arg2"\n")
#define ASM_SUB(arg1, arg2) fprintf(fp, "\tsubl\t"arg1", "arg2"\n")
#define ASM_MUL(arg1, arg2) fprintf(fp, "\timull\t"arg1", "arg2"\n")
#define ASM_DIV(arg1, arg2) fprintf(fp, "\tidivl\t"arg1", "arg2"\n")

#define CMP(arg1, arg2) fprintf(fp, "\tcmpl\t"arg1", "arg2"\n")
#define JMP(label) fprintf(fp, "\tjmp\t"label"\n")
#define JE(label) fprintf(fp, "\tje\t"label"\n")
#define JNE(label) fprintf(fp, "\tjne\t"label"\n")
#define JG(label) fprintf(fp, "\tjg\t"label"\n")
#define JGE(label) fprintf(fp, "\tjge\t"label"\n")
#define JL(label) fprintf(fp, "\tjl\t"label"\n")
#define JLE(label) fprintf(fp, "\tjle\t"label"\n")
#define JZ(label) fprintf(fp, "\tjz\t"label"\n")

#define STRINGIFY(x) #x

// Comparison types
#define CMP_EQUAL 0
#define CMP_NOT_EQUAL 1
#define CMP_GT 2
#define CMP_GTE 3
#define CMP_LT 4
#define CMP_LTE 5
#define CMP_UNDEFINED 6

/* Dear Professor or TA, if you see this, then please know how proud I am of these
macros I made. They are my babies. My beautiful creations. They are my everything.
I am nothing without them. My brain grew 3x in size when I made them from scratch.
Bask in their glory for their words are that of the holy scripture. */

static int label_count = 0;

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
    // Function label
    LABEL(node->entry->name);

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

    // Return statement
    generate_function_return(fp, node->return_node);
}

int generate_func_params(FILE *fp, AST_Function_Declaration_Params *node) {
    int offset = -4;

    if (node->params) {
        // Loop through all function parameters to move from registers to stack
        for (int i = 0; i < node->num_params; i++) {
            if (i > 1) {
                fail(fp, "Lol you thought we'd actually implement this to work with more than 2 params. You wrong.");
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
                    fail(fp, "codegen.c:generate_func_params() unknown type");
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
                    fail(fp, "codegen.c:generate_var_declarations() unknown type");
            }
        }
    }
}

void generate_statements(FILE *fp, AST_Statements *node) {
    if (node) {
        for (int i = 0; i < node->num_statements; i++) {
            AST_Node *statement = node->statements[i];
            generate_statement(fp, statement);
        }
    }
}

void generate_statement(FILE *fp, AST_Node *statement) {
    switch (statement->type) {
        case AST_ASSIGNMENT:
            {
                AST_Assignment *temp = (AST_Assignment*) statement;
                generate_assignment(fp, temp);
            }
            break;
        case AST_IF:
            {
                AST_If *temp = (AST_If*) statement;
                generate_if(fp, temp);
            }
            break;

        case AST_WHILE:
            {
                AST_While *temp = (AST_While*) statement;
                generate_while(fp, temp);
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
            fail(fp, "codegen.c:generate_statements() unknown type");
    }
}

void generate_assignment(FILE *fp, AST_Assignment *node) {
    generate_exp(fp, node->assign_value);
    int offset = node->entry->offset;
    MOV(EBX, MEM(offset, EBP));
}

void generate_if(FILE *fp, AST_If *node) {
    AST_Node *if_branch = node->if_branch;
    AST_Node *else_branch = node->else_branch;

    int else_label = get_label_num();

    // Check if condition
    int cmp_type = generate_exp_cmp(fp, node->condition);
    switch (cmp_type) {
        case CMP_EQUAL:
            {
                JNE(".L"STRINGIFY(else_label));
            }
            break;
        
        case CMP_NOT_EQUAL:
            {
                JE(".L"STRINGIFY(else_label));
            }
            break;
        
        case CMP_GT:
            {
                JLE(".L"STRINGIFY(else_label));
            }
            break;
        
        case CMP_GTE:
            {
                JL(".L"STRINGIFY(else_label));
            }
            break;
        
        case CMP_LT:
            {
                JGE(".L"STRINGIFY(else_label));
            }
            break;
        
        case CMP_LTE:
            {
                JG(".L"STRINGIFY(else_label));
            }
            break;
        
        case CMP_UNDEFINED:
            {
                JZ(".L"STRINGIFY(else_label));
            }
            break;
    }

    // Generate if branch
    generate_statement(fp, if_branch);

    // If there is no else branch, still need label to jump to outside of if
    // statement.
    LABEL(".L"STRINGIFY(else_label));

    // Generate else branch
    if (else_branch) {
        generate_statement(fp, else_branch);
    }
}

void generate_while(FILE *fp, AST_While *node) {
    // Print start label
    int start_label = get_label_num();
    int end_label = get_label_num();
    LABEL(".L"STRINGIFY(start_label));

    AST_Node *expression = node->condition;
    AST_Node *statement = node->while_branch;

    int cmp_type = generate_exp_cmp(fp, expression);
    switch (cmp_type) {
        case CMP_EQUAL:
            {
                JNE(".L"STRINGIFY(end_label));
            }
            break;
        
        case CMP_NOT_EQUAL:
            {
                JE(".L"STRINGIFY(end_label));
            }
            break;
        
        case CMP_GT:
            {
                JLE(".L"STRINGIFY(end_label));
            }
            break;
        
        case CMP_GTE:
            {
                JL(".L"STRINGIFY(end_label));
            }
            break;
        
        case CMP_LT:
            {
                JGE(".L"STRINGIFY(end_label));
            }
            break;
        
        case CMP_LTE:
            {
                JG(".L"STRINGIFY(end_label));
            }
            break;
        
        case CMP_UNDEFINED:
            {
                JZ(".L"STRINGIFY(end_label));
            }
            break;
    }

    generate_statement(fp, statement);

    // Go to start to check exp
    JMP(".L"STRINGIFY(start_label));

    // Print end label
    LABEL(".L"STRINGIFY(end_label));
}

void generate_function_return(FILE *fp, AST_Function_Return* node) {
    if (node) {
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
                fail(fp, "codegen.c:generate_var_declarations() unknown type");
        }
    }

    // Always print at the end of a function or is only thing printed if a void 
    // return type.
    POP(EBP);
    RET;
}

void generate_function_call(FILE *fp, AST_Function_Call *node) {
    char *func_name = node->entry->name;

    // Load params into registers
    for (int i = 0; i < node->num_params; i++) {
        if (i > 1) {
            fail(fp, "Lol you thought we'd actually implement this to work with more than 2 function args. You wrong.\n");
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

void generate_const(FILE *fp, AST_Const *node) {
    switch (node->const_type) {
        case INT_CONST_TYPE:
            {
                int num = node->val.int_value;
                MOV(IMM(num), EBX);
            }
            break;

        case FLOAT_CONST_TYPE:
            {
                // TODO
            }
            break;

        case CHAR_CONST_TYPE:
            {
                char val = node->val.char_value;
                MOV(IMM(val), EBX);
            }
            break;

        case STRING_CONST_TYPE:
            {
                int asm_label = node->val.string_value.asm_label;
                LEA(STR(asm_label), EBX);
                //TODO
            }
            break;

    }
}

void generate_arith(FILE *fp, AST_Arith *node) {
    generate_exp(fp, node->right);
    PUSH(EBX);
    generate_exp(fp, node->left);
    POP(ECX);

    switch (node->op) {
        ADD:
            {
                ASM_ADD(ECX, EBX);
            }
            break;

        SUB:
            {
                ASM_SUB(ECX, EBX);
            }
            break;

        MUL:
            {
                ASM_MUL(ECX, EBX);
            }
            break;

        DIV:
            {
                ASM_DIV(ECX, EBX);
            }
            break;
    }
}

void generate_exp(FILE *fp, AST_Node *node) {
    switch (node->type) {
        AST_EQUAL:
            {
                
            }
            break;

        AST_RELAT:
            {
                
            }
            break;

        AST_ARITH:
            {
                AST_Arith *temp = (AST_Arith*) node;
                generate_arith(fp, temp);
            }
            break;

        AST_UNARY:
            {
                
            }
            break;

        AST_IDENTIFIER_CONTAINER:
            {
                // Load var from stack to %ebx
                AST_Identifier_Container *temp = (AST_Identifier_Container*) node;
                int offset = temp->entry->offset;
                MOV(MEM(offset, EBP), EBX);
            }
            break;

        AST_FUNC_CALL:
            {
                AST_Function_Call *temp = (AST_Function_Call*) node;
                generate_function_call(fp, temp);
                // Move func call result to exp result register
                MOV(EAX, EBX);
            }
            break;

        AST_CONST:
            {
                AST_Const *temp = (AST_Const*) node;
                generate_const(fp, temp);
            }
            break;

        default:
            fail(fp, "codegen.c:generate_exp() unknown type");
    }
}

/*
 * Result will ALWAYS be in %ebx.
 */
int generate_exp_cmp(FILE *fp, AST_Node *node) {
    switch (node->type) {
        AST_EQUAL:
            {
                
            }
            break;

        AST_RELAT:
            {

            }
            break;

        AST_ARITH:
            {

            }
            break;

        AST_UNARY:
            {

            }
            break;

        AST_IDENTIFIER_CONTAINER:
            {

            }
            break;

        AST_FUNC_CALL:
            {
                AST_Function_Call *temp = (AST_Function_Call*) node;
                generate_function_call(fp, temp);
                // Move func call result to exp result register
                MOV(EAX, EBX);
                return CMP_UNDEFINED;
            }
            break;

        AST_CONST:
            {
                AST_Const *temp = 
            }
            break;

        default:
            fail(fp, "codegen.c:generate_exp() unknown type");
    }
}

int get_label_num() {
    label_count++;
    return label_count;
}

void fail(FILE *fp, char *err_message) {
    printf("[ERROR] %s\n", err_message);
    fclose(fp);
    exit(1);
}