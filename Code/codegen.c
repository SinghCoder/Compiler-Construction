#include "codegen.h"
#include <string.h>

void arithexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */
    
    /* 
        Integers are returned in rax or rdx:rax, and floating point values are returned in xmm0 or xmm1:xmm0
        Note: The floating point instructions have an sd suffix (i.e. convert single s to double precision d)
    */

    type *var_type_ptr1 = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *var_type_ptr2 = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg2, quad.encl_fun_type_ptr, NULL);
    type *var_type_result = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    int offset1 = var_type_ptr1->offset;
    int offset2 = var_type_ptr2->offset;
    int offset_result = var_type_result->offset;

    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");

    if(var_type_ptr1->name == INTEGER)
    {
        switch(quad.op)
        {
            case PLUS_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    add RAX, RBX \n\
                                    mov [RBP + %d], RAX \n", offset1, offset2, offset_result);

            case MINUS_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    sub RAX, RBX \n\
                                    mov [RBP + %d], RAX \n", offset1, offset2, offset_result);

            case MUL_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    mul RBX \n\
                                    mov [RBP + %d], RAX \n", offset1, offset2, offset_result);

            case DIV_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    div RBX \n\
                                    mov [RBP + %d], RAX \n", offset1, offset2, offset_result);
        }
    }
    else if(var_type_ptr1->name == REAL)
    {
        switch(quad.op)
        {
            case PLUS_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM2, [RBP + %d]\n\
                                    addsd XMM0, XMM2 \n\
                                    mov [RBP + %d], XMM0 \n", offset1, offset2, offset_result);

            case MINUS_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM2, [RBP + %d]\n\
                                    subsd XMM0, XMM2 \n\
                                    mov [RBP + %d], XMM0 \n", offset1, offset2, offset_result);

            case MUL_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM2, [RBP + %d]\n\
                                    mulsd XMM2 \n\
                                    mov [RBP + %d], XMM0 \n", offset1, offset2, offset_result);

            case DIV_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM2, [RBP + %d]\n\
                                    divsd XMM2 \n\
                                    mov [RBP + %d], XMM0 \n", offset1, offset2, offset_result);
        }
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void relexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */

    type *var_type_ptr1 = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *var_type_ptr2 = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg2, quad.encl_fun_type_ptr, NULL);
    type *var_type_result = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    int offset1 = var_type_ptr1->offset;
    int offset2 = var_type_ptr2->offset;
    int offset_result = var_type_result->offset;

    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");

    if(var_type_ptr1->name == INTEGER)
    {
        // initialize temp to 0
        fprintf(assembly_file_ptr, 
                "\t\t\t\tmov RAX, 0\n\
                    mov RDX, 1\n\
                    mov [RBP + %d], RAX \n", offset_result);

        switch(quad.op)
        {
            case LT_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    cmp RAX, RBX \n\
                                    cmovl [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case LE_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    cmp RAX, RBX \n\
                                    cmovle [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case GT_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    cmp RAX, RBX \n\
                                    cmovg [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case GE_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    cmp RAX, RBX \n\
                                    cmovge [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case NE_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    cmp RAX, RBX \n\
                                    cmovne [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case EQ_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov RBX, [RBP + %d]\n\
                                    cmp RAX, RBX \n\
                                    cmove [RBP + %d], RDX \n", offset1, offset2, offset_result);
        }
    }
    else if(var_type_ptr1->name == REAL)
    {
        // initialize temp to 0
        fprintf(assembly_file_ptr, 
                "\t\t\t\tmov RAX, 0\n\
                    mov RDX, 1\n\
                    mov [RBP + %d], RAX \n", offset_result);

        switch(quad.op)
        {
            case LT_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM1, [RBP + %d]\n\
                                    cmp XMM0, XMM1 \n\
                                    cmovl [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case LE_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM1, [RBP + %d]\n\
                                    cmp XMM0, XMM1 \n\
                                    cmovle [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case GT_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM1, [RBP + %d]\n\
                                    cmp XMM0, XMM1 \n\
                                    cmovg [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case GE_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM1, [RBP + %d]\n\
                                    cmp XMM0, XMM1 \n\
                                    cmovge [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case NE_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM1, [RBP + %d]\n\
                                    cmp XMM0, XMM1 \n\
                                    cmovne [RBP + %d], RDX \n", offset1, offset2, offset_result);

            case EQ_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov XMM1, [RBP + %d]\n\
                                    cmp XMM0, XMM1 \n\
                                    cmove [RBP + %d], RDX \n", offset1, offset2, offset_result);
        }
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
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

    type *var_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *var_type_result = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    int offset = var_type_ptr->offset;
    int offset_result = var_type_result->offset;

    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");

    if(var_type_ptr->name == INTEGER)
    {
        switch(quad.op)
        {
            case UPLUS_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    mov [RBP + %d], RAX \n", offset, offset_result);

            case UMINUS_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov RAX, [RBP + %d]\n\
                                    neg RAX\n\
                                    mov [RBP + %d], RAX \n", offset, offset_result);
        }
    }
    else if(var_type_ptr->name == REAL)
    {
        switch(quad.op)
        {
            case UPLUS_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    mov [RBP + %d], XMM0 \n", offset, offset_result);

            case UMINUS_OP: fprintf(assembly_file_ptr, 
                                "\t\t\t\tmov XMM0, [RBP + %d]\n\
                                    neg XMM0\n\
                                    mov [RBP + %d], XMM0 \n", offset, offset_result);
        }
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");

}

void input_code_gen(quad_node quad){
    /**
     * @brief Check the variable type
     * If primitive, generate code accordingly
     * and if array, take multiple inputs
     * assign the results back to corresponding memory locations
     */
    /**
     * MOV RDI, fmt
     * MOV RSI, val1
     * MOV RDX, val2
     * MOV RCX, val3..
     * MOV RAX, 0
     * call scanf
     */
    type *var_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    print_a_type(var_type_ptr);
    int offset = var_type_ptr->offset;
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    switch(var_type_ptr->name){
        case INTEGER:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
        }
        break;
        case REAL:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, real_fmt		;first arg, format \"%%lf\" \n");
        }
        break;
        case BOOLEAN:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, bool_fmt		;first arg, format \"%%s\" \n");
        }
        break;
        case ARRAY:
        {

        }
        break;
        default:
        {
            fprintf(assembly_file_ptr, "\t\t\t\t		;type error, cannot take input \"%%d\" \n");
        }
        break;
    }
    fprintf(assembly_file_ptr, 
            "\t\t\t\tmov RDX, RBP\n\
                sub RDX, %d     ; make RDX to point at location of variable on the stack\n\
                mov RSI, RDX \n\
                mov RAX, 0 \n\
                call scanf \n\
                pop_all\n", offset);
}

void output_code_gen(quad_node quad){
    /**
     * @brief Check variable type in symbol table
     * If primitive, just output the value
     * If array, output all the values
     * If #true/#false, print true false strings
     */
    type *var_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    print_a_type(var_type_ptr);
    int offset = var_type_ptr->offset;
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    switch(var_type_ptr->name){
        case INTEGER:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, RBP\n\
                sub RDX, %d     ; make RDX to point at location of variable on the stack\n\
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf\n\
                mov RAX, 0 \n\
                call printf \n\
                pop_all\n", offset);
        }
        break;
        case REAL:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, real_fmt		;first arg, format \"%%lf\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, RBP\n\
                sub RDX, %d     ; make RDX to point at location of variable on the stack\n\
                mov RSI, [RDX]  ; for real, value stored at offset should be passed to printf\n\
                mov RAX, 1      ; for real, number of floating point regs passed here = 1\n\
                call printf \n\
                pop_all\n", offset);
        }
        break;
        case BOOLEAN:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, bool_fmt		;first arg, format \"%%s\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, RBP\n\
                sub RDX, %d     ; make RDX to point at location of variable on the stack\n\
                mov RSI, RDX    ; for strings, pointer is passes as arguement to printf\n\
                mov RAX, 0 \n\
                call printf \n\
                pop_all\n", offset);
        }
        break;
        case ARRAY:
        {

        }
        break;
        default:
        {
            fprintf(assembly_file_ptr, "\t\t\t\t		;type error, cannot take input \"%%d\" \n");
        }
        break;
    }    
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
     * MOV [RBP + arr's offset], RBP + arr's offset + width_ptr
     */
    type *arr_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    int offset = arr_type_ptr->offset;
    fprintf(assembly_file_ptr, "\t\t\t\tMOV RAX, RBP\n");
    fprintf(assembly_file_ptr, "\t\t\t\tSUB RAX, %d\n", offset);
    fprintf(assembly_file_ptr, "\t\t\t\tMOV RBX, RAX\n");
    fprintf(assembly_file_ptr, "\t\t\t\tSUB RAX, %d\n", WIDTH_POINTER);
    fprintf(assembly_file_ptr, "\t\t\t\tMOV [RBX], RAX\n");
}

void fn_space_code_gen(quad_node quad){
    /**
     * @brief Reserve number of bytes equal to offset value
     * of this function, Also output function's label
     */

    fprintf(assembly_file_ptr, "%s:\n", quad.arg1);
    int offset = quad.encl_fun_type_ptr->typeinfo.module.curr_offset;
    int alignment = (16 - (offset % 16)) % 16;
    fprintf(assembly_file_ptr, "\t\t\t\tSUB RSP, %d     ; align RSP to 16 boundary to enable calls to scanf, etc\n", alignment);
    fprintf(assembly_file_ptr, "\t\t\t\tENTER %d, 0\n", offset);
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
    fprintf(assembly_file_ptr, "extern printf, scanf\n");
    fprintf(assembly_file_ptr, "section .data\n");
    fprintf(assembly_file_ptr, "\t\tffour_reg_fmt: db `RAX = %%ld, RBX = %%ld, RCX = %%ld, RDX = %%ld`, 10, 0\n");
    fprintf(assembly_file_ptr, "\t\tlfour_reg_fmt: db `RSP = %%ld, RBP = %%ld, RSI = %%ld, RDI = %%ld\\n`, 10, 0\n");
    fprintf(assembly_file_ptr, "\t\tint_fmt: db \"%%d\", 0\n");
    fprintf(assembly_file_ptr, "\t\treal_fmt: db \"%%lf\", 0\n");
    fprintf(assembly_file_ptr, "\t\tbool_fmt: db \"%%s\", 0\n");
    fprintf(assembly_file_ptr, "\t\tstr_fmt: db \"%%s\", 0\n");
    
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
                    mov RDI, ffour_reg_fmt		; first arg, format \n\
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
                    mov RDI, lfour_reg_fmt		; first arg, format \n\
                    pop RSI \n\
                    add RSI, 88 ; 88 because RSP has pushed additional 11 registers, so original value is 88 more than current\n\
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
                fprintf(assembly_file_ptr, "%s:\n",  quadruples[i].arg1);
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
                fprintf(assembly_file_ptr, "\t\t\t\tLEAVE\n");
                int offset = quadruples[i].encl_fun_type_ptr->typeinfo.module.curr_offset;
                int alignment = (16 - (offset % 16)) % 16;
                fprintf(assembly_file_ptr, "\t\t\t\tADD RSP, %d     ; align RSP back to intial position\n", alignment);
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