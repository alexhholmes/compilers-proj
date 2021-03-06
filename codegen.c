#include "codegen.h"
#include "symtab.h"
#include "ast.h"
#include "tinylex.tab.h"

#include <stdio.h>
#include <stdlib.h>

#define EAX "%%eax"
#define EBX "%%ebx"
#define ECX "%%ecx"
#define EDX "%%edx"
#define EDI "%%edi"
#define ESI "%%esi"
#define EBP "%%ebp"
#define ESP "%%esp"
#define EIP "%%esp"

#define LABEL(str, name) fprintf(fp, "%s%d:\n", str, name)
#define PUSH(reg) fprintf(fp, "\tpushl\t"reg"\n")
#define POP(reg) fprintf(fp, "\tpopl\t"reg"\n")
#define NEG(reg) fprintf(fp, "\tnegl\t"reg"\n")
#define CALL(func) fprintf(fp, "\tcall\t%s\n", func)
#define RET fprintf(fp, "\tret\n")
#define MOV(src, dest) fprintf(fp, "\tmovl\t"src", "dest"\n")
#define LEA(arg1, arg2) fprintf(fp, "\tleal\t"arg1", "arg2"\n")
#define ASM_ADD(arg1, arg2) fprintf(fp, "\taddl\t"arg1", "arg2"\n")
#define ASM_SUB(arg1, arg2) fprintf(fp, "\tsubl\t"arg1", "arg2"\n")
#define ASM_MUL(arg1, arg2) fprintf(fp, "\timull\t"arg1", "arg2"\n")
#define ASM_DIV(arg1, arg2) fprintf(fp, "\tidivl\t"arg1", "arg2"\n")

#define CMP(arg1, arg2) fprintf(fp, "\tcmpl\t"arg1", "arg2"\n")
#define JMP(label) fprintf(fp, "\tjmp\t.L%d\n", label)
#define JE(label) fprintf(fp, "\tje\t.L%d\n", label)
#define JNE(label) fprintf(fp, "\tjne\t.L%d\n", label)
#define JG(label) fprintf(fp, "\tjg\t.L%d\n", label)
#define JGE(label) fprintf(fp, "\tjge\t.L%d\n", label)
#define JL(label) fprintf(fp, "\tjl\t.L%d\n", label)
#define JLE(label) fprintf(fp, "\tjle\t.L%d\n", label)
#define JZ(label) fprintf(fp, "\tjz\t.L%d\n", label)

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

extern AST_Node *ast_head;

static int label_count = 0;
static int offset;
int string_label_counter = 0;

void generate_code() {
    FILE *fp;
    fp = fopen("output.s", "w");

    if (fp == NULL) {
        printf("File %s cannot be opened! Exiting...", "output.asm");
        exit(1);
    }

    generate_string_declarations(fp);
    fprintf(fp, "\n");
    fprintf(fp, "\t.globl\tmain\n");
    AST_Function_Declarations *node = (AST_Function_Declarations*) ast_head;
    generate_func_deflist(fp, node);

    fclose(fp);
}

void generate_string_declarations(FILE *fp) {
    fprintf(fp, "\t.text\n");

    // String label number
    int string_num = 0;

    for (int i = 0; i < string_label_counter; i++) {
        char *string_const = string_consts[i];
        fprintf(fp, ".LC%d:\n", string_num);
        fprintf(fp, "\t.string %s\n", string_const);
    }
}

void append_string_const(char *string_value) {
    if (!string_consts) {
        // First 
        string_consts = (char**) malloc(sizeof(char*));
        string_consts[0] = string_value;
        string_label_counter++;
    } else {
        string_consts = (char**) realloc(string_consts, (string_label_counter + 1) * sizeof(char*));
        string_consts[string_label_counter] = string_value;
        string_label_counter++;
    }
}

void generate_func_deflist(FILE *fp, AST_Function_Declarations *node) {
    for (int i = 0; i < node->func_declaration_count; i++) {
        AST_Function_Declaration *temp = (AST_Function_Declaration*) node->func_declarations[i];
        generate_func_def(fp, temp);
        fprintf(fp, "\n");
    }
}

void generate_func_def(FILE *fp, AST_Function_Declaration *node) {
    // Function label
    char *name = node->entry->name;
    fprintf(fp, "%s:\n", name);

    // Store %ebp and set it to %esp value
    PUSH(EBP);
    MOV(ESP, EBP);

    AST_Function_Declaration_Params *params = (AST_Function_Declaration_Params*)node->params;
    AST_Var_Declarations *var_decs = (AST_Var_Declarations*)node->var_declarations;

    // Calculate param offset
    int param_offset = 1;
    if (params) param_offset += params->num_params;
    param_offset *= 4;

    // Calculate stack pointer offset
    offset = 0; 
    if (var_decs) offset += var_decs->num_vars; 
    offset *= 4; 

    // Only offset stack pointer if we have declared variables to store on stack
    if (offset != 0) {
        fprintf(fp, "\tsubl\t$%d, "ESP"\n", offset);
    }

    generate_func_params(fp, params);

    int curr_var_offset = -4;
    generate_var_declarations(fp, var_decs, curr_var_offset);

    // Generate statements
    AST_Statements *statements = (AST_Statements*) node->statements;
    generate_statements(fp, statements);

    // Return statement
    AST_Function_Return *return_node = (AST_Function_Return*) node->return_node;
    generate_function_return(fp, return_node);
}

void generate_func_params(FILE *fp, AST_Function_Declaration_Params *node) {
    int offset = 8;

    if (node && node->params) {
        // Loop through all function parameters to move from registers to stack
        for (int i = 0; i < node->num_params; i++) {

            Param param = node->params[i];
            switch (param.param_type) {
                case INT_TYPE:
                    {
                        param.entry->offset = offset;
                        offset += 4;
                    }
                    break;
                case FLOAT_TYPE:
                    {
                        // TODO
                    }
                    break;
                case CHAR_TYPE:
                    {
                        param.entry->offset = offset;
                        offset += 4;
                    }
                    break;
                default:
                    fail(fp, "codegen.c:generate_func_params() unknown type");
            }
        }
    }
}

void generate_var_declarations(FILE *fp, AST_Var_Declarations *node, int offset) {
    if (node && node->var_declarations) {
        // Loop through all variable declarations and move constant values to stack
        for (int i = 0; i < node->num_vars; i++) {
            AST_Var_Declaration *var = (AST_Var_Declaration*) node->var_declarations[i];
            switch (var->data_type) {
                case INT_TYPE:
                    {
                        Symbol *entry = var->entry;
                        entry->offset = offset;
                        fprintf(fp, "\tmovl\t$%d, %d("EBP")\n", entry->value.int_val, offset);
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
                        fprintf(fp, "\tmovl\t$%d, %d("EBP")\n", entry->value.char_val, offset);
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
                AST_Statements *temp = (AST_Statements*) statement;
                generate_statements(fp, temp);
            }
            break;

        default:
            fail(fp, "codegen.c:generate_statements() unknown type");
    }
}

void generate_assignment(FILE *fp, AST_Assignment *node) {
    generate_exp(fp, node->assign_value);
    int offset = node->entry->offset;
    fprintf(fp, "\tmovl\t"EBX", %d("EBP")\n", offset);
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
                JNE(else_label);

            }
            break;
        
        case CMP_NOT_EQUAL:
            {
                JE(else_label);
            }
            break;
        
        case CMP_GT:
            {
                JLE(else_label);
            }
            break;
        
        case CMP_GTE:
            {
                JL(else_label);
            }
            break;
        
        case CMP_LT:
            {
                JGE(else_label);
            }
            break;
        
        case CMP_LTE:
            {
                JG(else_label);
            }
            break;
        
        case CMP_UNDEFINED:
            {
                JZ(else_label);
            }
            break;
    }

    // Generate if branch
    generate_statement(fp, if_branch);

    // If there is no else branch, still need label to jump to outside of if
    // statement.
    LABEL(".L", else_label);

    // Generate else branch
    if (else_branch) {
        generate_statement(fp, else_branch);
    }
}

void generate_while(FILE *fp, AST_While *node) {
    // Print start label
    int start_label = get_label_num();
    int end_label = get_label_num();
    LABEL(".L", start_label);

    AST_Node *expression = node->condition;
    AST_Node *statement = node->while_branch;

    int cmp_type = generate_exp_cmp(fp, expression);
    switch (cmp_type) {
        case CMP_EQUAL:
            {
                JNE(end_label);
            }
            break;
        
        case CMP_NOT_EQUAL:
            {
                JE(end_label);
            }
            break;
        
        case CMP_GT:
            {
                JLE(end_label);
            }
            break;
        
        case CMP_GTE:
            {
                JL(end_label);
            }
            break;
        
        case CMP_LT:
            {
                JGE(end_label);
            }
            break;
        
        case CMP_LTE:
            {
                JG(end_label);
            }
            break;
        
        case CMP_UNDEFINED:
            {
                JZ(end_label);
            }
            break;
    }

    generate_statement(fp, statement);

    // Go to start to check exp
    JMP(start_label);

    // Print end label
    LABEL(".L", end_label);
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
                fail(fp, "codegen.c:generate_function_return() unknown type");
        }
    }

    // Always print at the end of a function or is only thing printed if a void 
    // return type.
    if (offset != 0) {
        fprintf(fp, "\taddl\t$%d, "ESP"\n", offset);
    }
    POP(EBP);
    RET;
}

void generate_function_call(FILE *fp, AST_Function_Call *node) {
    char *func_name = node->entry->name;
    int offset = 0;

    // Load params onto the stack
    for (int i = node->num_params - 1; i >= 0; i--) {
        AST_Function_Call_Params *params = (AST_Function_Call_Params*) node->params;
        AST_Node *param = params->params[i];
        if (param->type == AST_CONST && ((AST_Const*) param)->const_type == STRING_CONST_TYPE) {
            AST_Const *constant = (AST_Const*) param;

            int label = constant->val.string_value.asm_label;
            fprintf(fp, "\tcall\t__x86.get_pc_thunk.ax\n");
            fprintf(fp, "\taddl\t$_GLOBAL_OFFSET_TABLE_, "EAX"\n");
            fprintf(fp, "\tleal\t.LC%d@GOTOFF("EAX"), "EDX"\n", label);
            PUSH(EDX);
        } else if (param->type == AST_IDENTIFIER_CONTAINER && ((AST_Identifier_Container *) param)->entry->passing == BY_REFER) {
            AST_Identifier_Container *identifier = (AST_Identifier_Container*) param;
            
            int stack_offset = identifier->entry->offset;
            fprintf(fp, "\tleal\t%d("EBP"), "EDX"\n", stack_offset);
            PUSH(EDX);
        } else {
            generate_exp(fp, param);
            
            PUSH(EBX);
        }
        offset += 4;
    }

    CALL(func_name);
    fprintf(fp, "\taddl\t$%d, "ESP"\n", offset);
}

void generate_const(FILE *fp, AST_Const *node) {
    switch (node->const_type) {
        case INT_CONST_TYPE:
            {
                int num = node->val.int_value;
                fprintf(fp, "\tmovl\t$%d, "EBX"\n", num);
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
                fprintf(fp, "\tmovl\t$%d, "EBX"\n", val);
            }
            break;

        case STRING_CONST_TYPE:
            {
                int asm_label = node->val.string_value.asm_label;
                // fprintf(fp, "\tleal\t%d("EBP"), "EBX"\n", asm_label);
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
        case ADD:
            {
                ASM_ADD(ECX, EBX);
            }
            break;

        case SUB:
            {
                ASM_SUB(ECX, EBX);
            }
            break;

        case MUL:
            {
                ASM_MUL(ECX, EBX);
            }
            break;

        case DIV:
            {
                ASM_DIV(ECX, EBX);
            }
            break;
    }
}

void generate_equal(FILE *fp, AST_Equal *node) {
    generate_exp(fp, node->right);
    PUSH(EBX);
    generate_exp(fp, node->left);
    POP(ECX);

    if (node->op == EQ) {
        CMP(EBX, ECX);
    } else {
        //TODO
    }
}

void generate_exp(FILE *fp, AST_Node *node) {
    switch (node->type) {
        case AST_EQUAL:
            {
                AST_Equal *temp = (AST_Equal*) node; 
                // TODO
            }
            break;

        case AST_RELAT:
            {
                //TODO
            }
            break;

        case AST_ARITH:
            {
                AST_Arith *temp = (AST_Arith*) node;
                generate_arith(fp, temp);
            }
            break;

        case AST_UNARY:
            {
                AST_Unary *temp = (AST_Unary*) node;
                generate_exp(fp, temp->expression);
                if (temp->sign == NEGATIVE) {
                    NEG(EBX);
                }
            }
            break;

        case AST_IDENTIFIER_CONTAINER:
            {
                // Load var from stack to %ebx
                AST_Identifier_Container *temp = (AST_Identifier_Container*) node;
                int offset = temp->entry->offset;
                fprintf(fp, "\tmovl\t%d("EBP"), "EBX"\n", offset);
            }
            break;

        case AST_FUNC_CALL:
            {
                AST_Function_Call *temp = (AST_Function_Call*) node;
                generate_function_call(fp, temp);
                // Move func call result to exp result register
                MOV(EAX, EBX);
            }
            break;

        case AST_CONST:
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
        case AST_EQUAL:
            {
                AST_Equal *temp = (AST_Equal*) node; 
                generate_exp(fp, temp->right);
                PUSH(EBX);
                generate_exp(fp, temp->left);
                POP(ECX);
                CMP(EBX, ECX);
                if (temp->op == EQ) {
                    return CMP_EQUAL; 
                } else {
                    return CMP_NOT_EQUAL;
                }
            }
            break;

        case AST_RELAT:
            {
                AST_Relat *temp = (AST_Relat*) node; 
                generate_exp(fp, temp->right);
                PUSH(EBX);
                generate_exp(fp, temp->left);
                POP(ECX);
                CMP(EBX, ECX);
                if (temp->op == GREATER_EQUAL) {
                    return CMP_GTE;  
                } else if (temp->op == GREATER) {
                    return CMP_GT;
                } else if (temp->op == LESS) {
                    return CMP_LT;
                } else {
                    return CMP_LTE;
                }
            }
            break;

        case AST_ARITH:
            {
                AST_Arith *temp = (AST_Arith*) node;
                generate_exp(fp, temp->right);
                PUSH(EBX);
                generate_exp(fp, temp->left);
                POP(ECX); 
                CMP(EBX, ECX); 
                return CMP_UNDEFINED;
            }
            break;

        case AST_UNARY:
            {
                AST_Unary *temp = (AST_Unary*) node; 
                generate_exp(fp, temp->expression);
                if (temp->sign == NEGATIVE) {
                    NEG(EBX);
                }
                fprintf(fp, "\tcmpl\t"EBX", $%d)\n", 0);
                return CMP_UNDEFINED; 
            }
            break;

        case AST_IDENTIFIER_CONTAINER:
            {
                AST_Identifier_Container *temp = (AST_Identifier_Container*) node; 
                int type = temp->entry->token_type;
                int offset = temp->entry->offset;

                if (type == INT_TYPE) {
                    fprintf(fp, "\tmovl\t%d("EBP"), "EBX"\n", offset);
                    fprintf(fp, "\tcmpl\t"EBX", $%d)\n", 0);
                    return CMP_UNDEFINED; 
                } else if (type == FLOAT_TYPE) {
                    // TODO
                } else if (type == CHAR_TYPE) {
                    fprintf(fp, "\tmovl\t%d("EBP"), "EBX"\n", offset);
                    fprintf(fp, "\tcmpl\t"EBX", $%d)\n", 0);
                    return CMP_UNDEFINED; 
                }
            }
            break;

        case AST_FUNC_CALL:
            {
                AST_Function_Call *temp = (AST_Function_Call*) node;
                generate_function_call(fp, temp);
                // Move func call result to exp result register
                MOV(EAX, EBX);
                return CMP_UNDEFINED;
            }
            break;

        case AST_CONST:
            {
                AST_Const *temp = (AST_Const*) node;
                int type = temp->const_type;

                if (type == INT_CONST_TYPE) {
                    int val = temp->val.int_value;
                    fprintf(fp, "\tmovl\t$%d, "EBX"\n", val);
                    fprintf(fp, "\tmovl\t$%d, "ECX"\n", 0);
                    fprintf(fp, "\tcmpl\t"EBX", "ECX"\n");
                    return CMP_UNDEFINED;
                } else if (type == FLOAT_CONST_TYPE) {
                    // TODO
                } else if (type == CHAR_CONST_TYPE) {
                    char val = temp->val.char_value;
                    fprintf(fp, "\tmovl\t$%d, "EBX"\n", val);
                    fprintf(fp, "\tmovl\t$%d, "ECX"\n", 0);
                    fprintf(fp, "\tcmpl\t"EBX", "ECX"\n");
                    return CMP_UNDEFINED;
                }
                return CMP_UNDEFINED;
            }
            break;

        default:
            fail(fp, "codegen.c:generate_exp() unknown type");
    }
    return CMP_UNDEFINED;
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