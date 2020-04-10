#include "codegen.h"
#include <string.h>

void arithexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */
}

void relexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */
}

void logexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */
}

void uexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */
}

void input_code_gen(quad_node quad){
    /**
     * @brief Check the variable type
     * If primitive, generate code accordingly
     * and if array, take multiple inputs
     * assign the results back to corresponding memory locations
     */
}

void output_code_gen(quad_node quad){
    /**
     * @brief Check variable type in symbol table
     * If primitive, just output the value
     * If array, output all the values
     * If #true/#false, print true false strings
     */
}

void assign_code_gen(quad_node quad){
    /**
     * @brief no special care required for assigning array to array
     * Simply copy value stored at second's location to first's location
     * take width number of bytes from source and copy to dest
     */
}

void jmp_if_true_code_gen(quad_node quad){
    /**
     * @brief True is represented as 1
     * So subtract one from the value
     * And jump if answer is zero(jz isntrn)
     */
}

void param_code_gen(quad_node quad){
    /**
     * @brief Push the parameter on the stack
     */
}

void index_copy_code_gen(quad_node quad){
    /**
     * @brief arr[i] := smth
     * get offset for arr[i]
     * store "smth" in the reg
     * then from reg to memory locn of arr[i]
     */
}

void array_access_code_gen(quad_node quad){
    /**
     * @brief smth := arr[i]
     * get offset for arr[i]
     * Store value from there to register
     * from register to locn of "smth"
     */
}

void dynamic_arr_space_code_gen(quad_node quad){
    /**
     * @brief Reserve somehow the n-m+1 * width_of_elem space
     * For this dynamic array
     */
}

void static_arr_space_code_gen(quad_node quad){
    /**
     * Space is already allocated, just store base address of first element at location of base address
     * MOV RSI, RBP + arr's offset
     * MOV [RSI], RSI + WIDTH_POINTER
     */
    // int offset = quad.encl_fun_type_ptr->typeinfo.module.curr_offset;
    // fprintf(assembly_file_ptr, "\t\t\t\tMOV RAX, RBP\n");
    // fprintf(assembly_file_ptr, "\t\t\t\tADD RAX, %d\n", offset);
    // fprintf(assembly_file_ptr, "\t\t\t\tADD RAX, %d\n", WIDTH_POINTER);
    // fprintf(assembly_file_ptr, "\t\t\t\tMOV [RSI], RAX\n");
}

void fn_space_code_gen(quad_node quad){
    /**
     * @brief Reserve number of bytes equal to offset value
     * of this function, Also output function's label
     */

    fprintf(assembly_file_ptr, "%s:\n", quad.arg1);
    if(strcmp(quad.arg1, "main") == 0){
        fprintf(assembly_file_ptr, "\t\t\t\tPUSH RBP\n");
        fprintf(assembly_file_ptr, "\t\t\t\tMOV RBP, RSP\n");
    }
    int offset = quad.encl_fun_type_ptr->typeinfo.module.curr_offset;
    fprintf(assembly_file_ptr, "\t\t\t\tSUB RSP, %d\n", offset);
    print_regs_code_gen();
}

void print_regs_code_gen(){
    fprintf(assembly_file_ptr, "\t\t\t\tprint_first_four\n");
    fprintf(assembly_file_ptr, "\t\t\t\tprint_last_four\n");
}

void exit_if_true_code_gen(quad_node quad){
    /**
     * @brief Check condition
     * If 1 comes output, exit call
     */
}

void generate_code(){    
    fprintf(assembly_file_ptr, "extern printf\n");
    fprintf(assembly_file_ptr, "section .data\n");
    fprintf(assembly_file_ptr, "\t\tfmt1: db \"RAX = %%ld, RBX = %%ld, RCX = %%ld, RDX = %%ld\", 10, 0\n");
    fprintf(assembly_file_ptr, "\t\tfmt2: db \"RSP = %%ld, RBP = %%ld, RSI = %%ld, RDI = %%ld\", 10, 0\n");    
    
    fprintf(assembly_file_ptr, "section .text\n");
    
    fprintf(assembly_file_ptr, 
            "\t\t\t\t%%macro push_all 0 \n\
                    push RAX \n\
                    push RBX \n\
                    push RCX \n\
                    push RDX \n\
                    push RSP \n\
                    push RBP \n\
                    push RSI \n\
                    push RDI \n\
                %%endmacro \n\n");
    
    fprintf(assembly_file_ptr, 
            "\t\t\t\t%%macro pop_all 0 \n\
                    pop RDI \n\
                    pop RSI \n\
                    pop RBP \n\
                    pop RSP \n\
                    pop RDX \n\
                    pop RCX \n\
                    pop RBX \n\
                    pop RAX \n\
                %%endmacro \n\n");

    fprintf(assembly_file_ptr, 
            "\t\t\t\t%%macro print_first_four 0 \n\
                    push_all\n\
                    push RDX \n\
                    push RCX \n\
                    push RBX \n\
                    push RAX \n\
                    mov RDI, fmt1		; first arg, format \n\
                    pop RSI \n\
                    pop RDX \n\
                    pop RCX \n\
                    pop R8 \n\
                    mov RAX, 0 \n\
                    call printf \n\
                    pop_all\n\
                %%endmacro \n\n");
    
    fprintf(assembly_file_ptr,
            "\t\t\t\t%%macro print_last_four 0 \n\
                    push_all\n\
                    push RDI \n\
                    push RSI \n\
                    push RBP \n\
                    push RSP \n\
                    mov RDI, fmt2		; first arg, format \n\
                    pop RSI \n\
                    pop RDX \n\
                    pop RCX \n\
                    pop R8 \n\
                    mov RAX, 0 \n\
                    call printf \n\
                    pop_all\n\
                %%endmacro \n\n");

    fprintf(assembly_file_ptr, "\t\t\t\tglobal main\n");

    for(int i = 0; i < quad_count; i++){
        switch(quadruples[i].op){
            case PLUS_OP:
            case MINUS_OP:
            case MUL_OP:
            case DIV_OP:
            {
                arithexpr_code_gen(quadruples[i]);
            }
            break;

            case GE_OP:
            case GT_OP:
            case LE_OP:
            case LT_OP:
            case EQ_OP:
            case NE_OP:
            {
                relexpr_code_gen(quadruples[i]);
            }
            break;

            case AND_OP:
            case OR_OP:
            {
                logexpr_code_gen(quadruples[i]);
            }
            break;

            case UMINUS_OP:
            case UPLUS_OP:
            {
                uexpr_code_gen(quadruples[i]);
            }
            break;

            case LABEL_OP:
            {
                fprintf(assembly_file_ptr, "\t%s:\n",  quadruples[i].arg1);
            }
            break;

            case INPUT_OP:
            {
                input_code_gen(quadruples[i]);
            }
            break;

            case OUTPUT_OP:
            {
                output_code_gen(quadruples[i]);
            }
            break;

            case ASSIGN_OP:
            {
                assign_code_gen(quadruples[i]);
            }
            break;

            case GOTO_UNCOND_OP:
            {
                fprintf(assembly_file_ptr, "\t\t\t\t%s %s\n",  tac_op_str[quadruples[i].op] ,quadruples[i].arg1);
            }
            break;

            case IF_TRUE_GOTO_OP:
            {
                jmp_if_true_code_gen(quadruples[i]);
            }
            break;

            case INP_PARAM_OP:
            case OUTP_PARAM_OP:
            {
                param_code_gen(quadruples[i]);
            }
            break;

            case PROC_CALL_OP:
            {
                fprintf(assembly_file_ptr, "\t\t\t\t%s %s\n", tac_op_str[quadruples[i].op], quadruples[i].arg1);
            }
            break;

            case INDEXED_COPY_OP:
            {
                index_copy_code_gen(quadruples[i]);
            }
            break;

            case ARRAY_ACCESS_OP:
            {
                array_access_code_gen(quadruples[i]);
            }
            break;

            case DECLARE_DYN_ARR_OP:
            {
                dynamic_arr_space_code_gen(quadruples[i]);
            }
            break;

            case DECLARE_STATIC_ARR_OP:
            {
                static_arr_space_code_gen(quadruples[i]);
            }
            break;
            case RETURN_OP:
            {                
                int offset = quadruples[i].encl_fun_type_ptr->typeinfo.module.curr_offset;
                fprintf(assembly_file_ptr, "\t\t\t\tADD RSP, %d\n", offset);
                fprintf(assembly_file_ptr, "\t\t\t\t%s\n\n", tac_op_str[quadruples[i].op]);
            }
            break;

            case FN_DEFN_OP:
            {
                fn_space_code_gen(quadruples[i]);
            }
            break;

            case EXIT_PROGRAM_IF_TRUE_OP:
            {
                exit_if_true_code_gen(quadruples[i]);
            }
            break;
        }

    }
}