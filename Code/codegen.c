/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#include "codegen.h"
#include <malloc.h>
#include <string.h>
#include "stackADT.h"

void arithexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */
    
    /* 
        Integers are returned in rax or rdx:rax, and floating point values are returned in xmm0 or xmm1:xmm0
        Note: The floating point instructions have an sd suffix (i.e. convert single s to double precision d)
    */

    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *arg2_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg2, quad.encl_fun_type_ptr, NULL);
    type *res_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    char *arg1_str, *arg2_str;
    
    arg1_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);
    arg2_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_arg1 = 0;
    
    if(arg1_type_ptr){
        offset_arg1 = arg1_type_ptr->offset_used;
        snprintf(arg1_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg1);
    }
    else{
        strncpy(arg1_str, quad.arg1, MAX_LEXEME_LEN);
    }
    
    int offset_arg2 = 0;
    
    if(arg2_type_ptr){
        offset_arg2 = arg2_type_ptr->offset_used;
        snprintf(arg2_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg2);
    }
    else{
        strncpy(arg2_str, quad.arg2, MAX_LEXEME_LEN);
    }
    
    int offset_result = res_type_ptr->offset_used;
    
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");

    if(res_type_ptr->name == INTEGER)
    {
        switch(quad.op)
        {
            case PLUS_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Addition of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                add RAX, RBX \n\
                mov [RBP - %d], RAX \n", arg1_str, arg2_str, offset_result);
            break;

            case MINUS_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Subtraction of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                sub RAX, RBX \n\
                mov [RBP - %d], RAX \n", arg1_str, arg2_str, offset_result);
            break;
            
            case MUL_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Multiplication of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                mul RBX \n\
                mov [RBP - %d], RAX \n", arg1_str, arg2_str, offset_result);
            break;

            case DIV_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Division of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, 0\n\
                mov RAX, %s\n\
                mov RBX, %s\n\
                div RBX \n\
                mov [RBP - %d], RAX \n", arg1_str, arg2_str, offset_result);
            break;
            
        }
    }
    else if(res_type_ptr->name == REAL)
    {
        if(!arg1_type_ptr)  // immediate value
        {
            char *real1_str = newlabel();
            fprintf(assembly_file_ptr,"\t\t\t\tsection .data\n");
            fprintf(assembly_file_ptr,"\t\t\t\t%s: dq %s\n", real1_str, arg1_str);
            fprintf(assembly_file_ptr,"\t\t\t\tsection .text\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmovsd XMM0, [%s]\n", real1_str);            
        }
        else
            fprintf(assembly_file_ptr, "\t\t\t\tmovsd XMM0, %s\n", arg1_str);

        if(!arg2_type_ptr)  // immediate value
        {
            char *real2_str = newlabel();
            fprintf(assembly_file_ptr,"\t\t\t\tsection .data\n");
            fprintf(assembly_file_ptr,"\t\t\t\t%s: dq %s\n", real2_str, arg2_str);
            fprintf(assembly_file_ptr,"\t\t\t\tsection .text\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmovsd XMM2, [%s]\n", real2_str);
        }
        else
            fprintf(assembly_file_ptr, "\t\t\t\tmovsd XMM2, %s\n", arg2_str);
        
        switch(quad.op)
        {
            case PLUS_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Addition of reals\n");
            fprintf(assembly_file_ptr,"\t\t\t\taddsd XMM0, XMM2 \n\
                movsd [RBP - %d], XMM0 \n",offset_result);
            break;

            case MINUS_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Subtraction of reals\n");
            fprintf(assembly_file_ptr,"\t\t\t\tsubsd XMM0, XMM2 \n\
                movsd [RBP - %d], XMM0 \n", offset_result);
            break;

            case MUL_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Multiplication of reals\n");
            fprintf(assembly_file_ptr,"\t\t\t\tmulsd XMM0, XMM2 \n\
                movsd [RBP - %d], XMM0 \n",offset_result);
            break;

            case DIV_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Division of reals\n");
            fprintf(assembly_file_ptr,"\t\t\t\tdivsd XMM0, XMM2 \n\
                movsd [RBP - %d], XMM0 \n", offset_result);
            break;
            
        }
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

token_name get_type_name(char *str){
    int len = strlen(str);
    for(int i=0; i<len; i++){
        if(str[i] == '.')
            return REAL;
    }
    return INTEGER;
}

void relexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */

    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *arg2_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg2, quad.encl_fun_type_ptr, NULL);
    type *res_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    char *arg1_str, *arg2_str;
    token_name type_name = TYPE_ERROR;
    
    arg1_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);
    arg2_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_arg1 = 0;
    
    if(arg1_type_ptr){
        offset_arg1 = arg1_type_ptr->offset_used;
        snprintf(arg1_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg1);
        type_name = arg1_type_ptr->name;
    }
    else{
        strncpy(arg1_str, quad.arg1, MAX_LEXEME_LEN);
    }
    
    int offset_arg2 = 0;
    
    if(arg2_type_ptr){
        offset_arg2 = arg2_type_ptr->offset_used;
        snprintf(arg2_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg2);
        type_name = arg2_type_ptr->name;
    }
    else{
        strncpy(arg2_str, quad.arg2, MAX_LEXEME_LEN);
    }

    if(type_name == TYPE_ERROR)
        type_name = get_type_name(arg1_str);

    int offset_result = res_type_ptr->offset_used;

    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");

    if(type_name == INTEGER || type_name == BOOLEAN)
    {
        // initialize temp to 0
        fprintf(assembly_file_ptr, "\t\t\t\tmov RCX, 0; initialize RCX with false\n\
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies\n");

        switch(quad.op)
        {
            case LT_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (LT) of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                cmp RAX, RBX \n\
                cmovl RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case LE_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (LE) of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                cmp RAX, RBX \n\
                cmovle RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case GT_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (GT) of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                cmp RAX, RBX \n\
                cmovg RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case GE_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (GE) of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                cmp RAX, RBX \n\
                cmovge RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case NE_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (NE) of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                cmp RAX, RBX \n\
                cmovne RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case EQ_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (EQ) of integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                cmp RAX, RBX \n\
                cmove RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;
        }
    }
    else if(type_name == REAL)
    {
        // initialize temp to 0
        fprintf(assembly_file_ptr,"\t\t\t\tmov RCX, 0; initialize RCX with false\n\
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies\n");

        switch(quad.op)
        {
            case LT_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (LT) of reals\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov XMM0, %s\n\
                mov XMM1, %s\n\
                cmp XMM0, XMM1 \n\
                cmovl RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case LE_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (LE) of reals\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov XMM0, %s\n\
                mov XMM1, %s\n\
                cmp XMM0, XMM1 \n\
                cmovle RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case GT_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (GT) of reals\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov XMM0, %s\n\
                mov XMM1, %s\n\
                cmp XMM0, XMM1 \n\
                cmovg RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case GE_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (GE) of reals\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov XMM0, %s\n\
                mov XMM1, %s\n\
                cmp XMM0, XMM1 \n\
                cmovge RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case NE_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (NE) of reals\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov XMM0, %s\n\
                mov XMM1, %s\n\
                cmp XMM0, XMM1 \n\
                cmovne RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;

            case EQ_OP:
            fprintf(assembly_file_ptr, "\t\t\t\t;Comparison (EQ) of reals\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov XMM0, %s\n\
                mov XMM1, %s\n\
                cmp XMM0, XMM1 \n\
                cmove RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
            break;
        }
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void logexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */
    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *arg2_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg2, quad.encl_fun_type_ptr, NULL);
    type *res_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    char *arg1_str, *arg2_str;
    
    arg1_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);
    arg2_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_arg1 = 0;
    
    if(arg1_type_ptr){
        offset_arg1 = arg1_type_ptr->offset_used;
        snprintf(arg1_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg1);
    }
    else{
        strncpy(arg1_str, quad.arg1, MAX_LEXEME_LEN);
    }
    
    int offset_arg2 = 0;
    
    if(arg2_type_ptr){
        offset_arg2 = arg2_type_ptr->offset_used;
        snprintf(arg2_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg2);
    }
    else{
        strncpy(arg2_str, quad.arg2, MAX_LEXEME_LEN);
    }
    
    int offset_result = res_type_ptr->offset_used;
    
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");

    fprintf(assembly_file_ptr, "\t\t\t\tmov RCX, 0; initialize RCX with false\n\
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies\n");

    if(quad.op == AND_OP){
        fprintf(assembly_file_ptr, "\t\t\t\t;Logical AND\n");
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                add RAX, RBX \n\
                cmp RAX, 2 \n\
                cmove RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
    }
    else{
        fprintf(assembly_file_ptr, "\t\t\t\t;Logical OR \n");
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov RBX, %s\n\
                sub RAX, RBX \n\
                cmp RAX, 0 \n\
                cmovne RCX, RDX \n\
                mov [RBP - %d], RCX \n", arg1_str, arg2_str, offset_result);
    }
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void uexpr_code_gen(quad_node quad){
    /**
     * @brief Check the operator and generate code
     */

    type *var_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *res_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    int offset_result = res_type_ptr->offset_used;

    char *var_str;
    
    var_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_var = 0;
    
    if(var_type_ptr){
        offset_var = var_type_ptr->offset_used;
        snprintf(var_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_var);
    }
    else{
        strncpy(var_str, quad.arg1, MAX_LEXEME_LEN);
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");

    if(var_type_ptr->name == INTEGER)
    {
        switch(quad.op)
        {
            case UPLUS_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Unary plus for integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                mov [RBP - %d], RAX \n", var_str, offset_result);
            break;
            
            case UMINUS_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Unary minus for integers\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n\
                neg RAX\n\
                mov [RBP - %d], RAX \n", var_str, offset_result);
            break;
        }
    }
    else if(var_type_ptr->name == REAL)
    {
        switch(quad.op)
        {
            case UPLUS_OP:
            fprintf(assembly_file_ptr, "\t\t\t\t;Unary plus for reals\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov XMM0, %s\n\
                mov [RBP - %d], XMM0 \n", var_str, offset_result);
            break;

            case UMINUS_OP: 
            fprintf(assembly_file_ptr, "\t\t\t\t;Unary minus for reals\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov XMM0, %s\n\
                neg XMM0\n\
                mov [RBP - %d], XMM0 \n", var_str, offset_result);
            break;
        }
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");

}

void one_input_code_gen(token_name type_name, int offset){    
    switch(type_name){
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
    }
    fprintf(assembly_file_ptr, 
            "\t\t\t\tmov RDX, RBP\n\
                sub RDX, %d     ; make RDX to point at location of variable on the stack\n\
                mov RAX, 0x0000_0000_ffff_ffff ;machine has sizeof(int) to be 4, for uniformity, We are taking\n\
                mov [RDX] , RAX ;our sizeof(int) to be 8, now scanf will just enter values in lower 32 bits\n\
                ;So, we are firstly clearing upper 32 bits of memory so as to access data properly later\n\
                mov RSI, RDX \n\
                mov RAX, 0 \n\
                align_16_rsp ;align RSP to 16 byte boundary for scanf call\n\
                call scanf \n\
                remove_align_16_rsp ;realign it to original position\n", offset);
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
    int offset = var_type_ptr->offset_used;
    int width;

    fprintf(assembly_file_ptr, "\t\t\t\t; Code to take input value(s) of %s\n", quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    switch(var_type_ptr->name){
        case INTEGER:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\"\n");
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Input: Enter an integer value for %s\"\n", quad.arg1);
            one_input_code_gen(INTEGER, offset);
        }
        break;
        case REAL:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\"\n");
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Input: Enter a real value for %s\"\n", quad.arg1);
            one_input_code_gen(REAL, offset);
        }
        break;
        case BOOLEAN:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\"\n");
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Input: Enter a boolean value for %s\"\n", quad.arg1);
            one_input_code_gen(BOOLEAN, offset);
        }
        break;
        case ARRAY:
        {
            int num_floating_regs = 0;
            char *prim_type;
            /**
             * @brief Code for format of input == base type
             */
            
            switch(var_type_ptr->typeinfo.array.primitive_type){
                case INTEGER:
                {
                    width = WIDTH_POINTER;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
                    prim_type = "integer";
                }
                break;
                case REAL:
                {
                    width = WIDTH_POINTER;
                    num_floating_regs = 1;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, real_fmt		;first arg, format \"%%d\" \n");
                    prim_type = "real";
                }
                break;
                case BOOLEAN:
                {
                    width = WIDTH_POINTER;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, bool_fmt		;first arg, format \"%%d\" \n");
                    prim_type = "boolean";
                }
            }

            /**
             * @brief Store count of elements in RCX
             */
            char *range_low, *range_high;
            int rlow_val = -1, rhigh_val = -1;
            if(var_type_ptr->typeinfo.array.is_dynamic.range_low){
                range_low = var_type_ptr->typeinfo.array.range_low.lexeme;
                type *rlow_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_low, quad.encl_fun_type_ptr, NULL);
                int offset = rlow_type->offset_used;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d] ; Load low range value from %s's location\n", offset, range_low);
            }
            else{
                rlow_val = var_type_ptr->typeinfo.array.range_low.value;
                range_low = (char *)malloc(sizeof(char) * MAX_LEXEME_LEN);
                snprintf(range_low, MAX_LEXEME_LEN, "%d", rlow_val);
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d ; Load low range value into RAX\n", rlow_val);                
            }

            if(var_type_ptr->typeinfo.array.is_dynamic.range_high){
                range_high = var_type_ptr->typeinfo.array.range_high.lexeme;
                type *rhigh_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_high, quad.encl_fun_type_ptr, NULL);
                int offset = rhigh_type->offset_used;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, [RBP - %d] ; Load high range value from %s's location\n", offset, range_high);
            }
            else{
                rhigh_val = var_type_ptr->typeinfo.array.range_high.value;
                range_high = (char *)malloc(sizeof(char) * MAX_LEXEME_LEN);
                snprintf(range_high, MAX_LEXEME_LEN, "%d", rhigh_val);
                fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, %d ; Load high range value into RBX\n", rhigh_val);                
            }

            fprintf(assembly_file_ptr, "\t\t\t\t;Store count of array elements in RCX\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RCX, RBX\n");
            fprintf(assembly_file_ptr, "\t\t\t\tsub RCX, RAX\n");
            fprintf(assembly_file_ptr, "\t\t\t\tadd RCX, 1  ; count = high_range - low_range + 1\n");
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str_no_new_line \"Input: Enter \"\n");
            
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RSI, RCX  ;count of array elems\n\
                push_all\n\
                mov RAX, 0 \n\
                align_16_rsp ;align RSP to 16 byte boundary for printf call\n\
                call printf \n\
                remove_align_16_rsp ;realign RSP\n\
                pop_all\n");

            fprintf(assembly_file_ptr, "\t\t\t\tprint_str_no_new_line \" number of array elements of %s type for range \"\n",prim_type);

            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RSI, RAX  ;count of array elems\n\
                push_all\n\
                mov RAX, 0 \n\
                align_16_rsp ;align RSP to 16 byte boundary for printf call\n\
                call printf \n\
                remove_align_16_rsp ;realign RSP\n\
                pop_all\n");
            
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str_no_new_line \" to \"\n");

            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RSI, RBX  ;count of array elems\n\
                push_all\n\
                mov RAX, 0 \n\
                align_16_rsp ;align RSP to 16 byte boundary for printf call\n\
                call printf \n\
                remove_align_16_rsp ;realign RSP\n\
                pop_all\n");

            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \" for array %s\"\n", quad.arg1);

            fprintf(assembly_file_ptr, "\t\t\t\t;taking input for array elements\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, [RBP - %d] ;address of arr[0] in RDX from contents of [%s]\n", offset, quad.arg1);            

            // emit loop label
            fprintf(assembly_file_ptr, "%s: ;scanf loop label\n", quad.cnstrct_code_begin);
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, 0x0000_0000_ffff_ffff ;machine has sizeof(int) to be 4, and we took 8, so clear upper 32 bits\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov [RDX], RAX ;clear upper 32 bits of array's element\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RSI, RDX ;address of variable to be scanned in RSI\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d ;num pf float regs\n", num_floating_regs);
            fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
            fprintf(assembly_file_ptr, "\t\t\t\talign_16_rsp ;align RSP to 16 byte boundary for scanf call\n");            
            fprintf(assembly_file_ptr, "\t\t\t\tcall scanf\n");
            fprintf(assembly_file_ptr, "\t\t\t\tremove_align_16_rsp ;realign it to original position\n");

            fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
            fprintf(assembly_file_ptr, "\t\t\t\tsub RDX, %d ;move pointer to next elem of array\n", width);
            fprintf(assembly_file_ptr, "\t\t\t\tdec RCX ;decrement num of elem count to be scanned\n");
            fprintf(assembly_file_ptr, "\t\t\t\tjnz %s\n", quad.cnstrct_code_begin);

        }
        break;
        default:
        {
            fprintf(assembly_file_ptr, "\t\t\t\t		;type error, cannot take input \"%%d\" \n");
        }
        break;
    }
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void one_var_output_code_gen(token_name type_name, int offset){
    switch(type_name){
        case INTEGER:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, RBP\n\
                sub RDX, %d     ; make RDX to point at location of variable on the stack\n\
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf\n\
                mov RAX, 0 \n\
                push_all\n\
                align_16_rsp ;align RSP to 16 byte boundary for printf call\n\
                call printf \n\
                remove_align_16_rsp ;realign RSP\n\
                pop_all\n", offset);
        }
        break;
        case REAL:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, real_fmt		;first arg, format \"%%lf\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, RBP\n\
                sub RDX, %d     ; make RDX to point at location of variable on the stack\n\
                movq XMM0, [RDX]  ; for real, value stored at offset should be passed to printf\n\
                mov RAX, 1      ; for real, number of floating point regs passed here = 1\n\
                push_all\n\
                align_16_rsp ;align RSP to 16 byte boundary for printf call\n\
                call printf \n\
                remove_align_16_rsp ;realign RSP\n\
                pop_all\n", offset);
        }
        break;
        case BOOLEAN:
        {
            char *bool_print_false_lbl = newlabel();
            char *bool_print_next_lbl = newlabel();
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, bool_fmt		;first arg, format \"%%s\" \n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, RBP\n\
                sub RDX, %d     ; make RDX to point at location of variable on the stack\n\
                mov RAX, [RDX]    \n\
                cmp RAX, 0 \n\
                jz %s\n\
                print_str_no_new_line \"true\" \n\
                jmp %s\n\
%s:\n\
                print_str_no_new_line \"false\" \n\
%s:\n", offset, bool_print_false_lbl, bool_print_next_lbl,  bool_print_false_lbl, bool_print_next_lbl);
        }
        break;
    }
}

void output_code_gen(quad_node quad){
    /**
     * @brief Check variable type in symbol table
     * If primitive, just output the value
     * If array, output all the values
     * If #true/#false, print true false strings
     */
    type *var_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    if(var_type_ptr == NULL){       // if a value has to be printed
        fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"%s\"\n", quad.arg1);
        return;
    }
    print_a_type(var_type_ptr);
    int offset = var_type_ptr->offset_used;
    int width;
    fprintf(assembly_file_ptr, "\t\t\t\t; Code to output value(s) of %s\n", quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\"\n");    
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str_no_new_line \"Output: \"\n");    
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");    
    switch(var_type_ptr->name){
        case INTEGER:
        {
            one_var_output_code_gen(INTEGER, offset);
        }
        break;
        case REAL:
        {
            one_var_output_code_gen(REAL, offset);
        }
        break;
        case BOOLEAN:
        {
            one_var_output_code_gen(BOOLEAN, offset);
        }
        break;
        case ARRAY:
        {
            int num_floating_regs = 0;
            /**
             * @brief Code for format of input == base type
             */
            token_name prim_type_name = var_type_ptr->typeinfo.array.primitive_type;
            switch(prim_type_name){
                case INTEGER:
                {
                    width = WIDTH_POINTER;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
                }
                break;
                case REAL:
                {
                    width = WIDTH_POINTER;
                    num_floating_regs = 1;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, real_fmt		;first arg, format \"%%d\" \n");
                }
                break;
                case BOOLEAN:
                {
                    width = WIDTH_POINTER;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, bool_fmt		;first arg, format \"%%d\" \n");
                }
            }

            /**
             * @brief Store count of elements in RCX
             */
            char *range_low, *range_high;
            int rlow_val = -1, rhigh_val = -1;
            if(var_type_ptr->typeinfo.array.is_dynamic.range_low){
                range_low = var_type_ptr->typeinfo.array.range_low.lexeme;
                type *rlow_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_low, quad.encl_fun_type_ptr, NULL);
                int offset = rlow_type->offset_used;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d] ; Load low range value from %s's location\n", offset, range_low);
            }
            else{
                rlow_val = var_type_ptr->typeinfo.array.range_low.value;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d ; Load low range value into RAX\n", rlow_val);                
            }

            if(var_type_ptr->typeinfo.array.is_dynamic.range_high){
                range_high = var_type_ptr->typeinfo.array.range_high.lexeme;
                type *rhigh_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_high, quad.encl_fun_type_ptr, NULL);
                int offset = rhigh_type->offset_used;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, [RBP - %d] ; Load high range value from %s's location\n", offset, range_high);
            }
            else{
                rhigh_val = var_type_ptr->typeinfo.array.range_high.value;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, %d ; Load high range value into RBX\n", rhigh_val);                
            }

            fprintf(assembly_file_ptr, "\t\t\t\t;Store count of array elements in RCX\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RCX, RBX\n");
            fprintf(assembly_file_ptr, "\t\t\t\tsub RCX, RAX\n");
            fprintf(assembly_file_ptr, "\t\t\t\tadd RCX, 1  ; count = high_range - low_range + 1\n");

            fprintf(assembly_file_ptr, "\t\t\t\t;printing array elements\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmov RDX, [RBP - %d] ;address of arr[0] in RDX from contents of [%s]\n", offset, quad.arg1);            

            // emit loop label
            fprintf(assembly_file_ptr, "%s: ;printf loop label\n", quad.cnstrct_code_begin);
            if(prim_type_name != REAL)
                fprintf(assembly_file_ptr, "\t\t\t\tmov RSI, [RDX] ;contents of variable to be printed in RSI\n");
            else
                fprintf(assembly_file_ptr, "\t\t\t\tmovq XMM0, [RDX] ;contents of variable to be printed in RSI\n");
            
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d ;num of float regs\n", num_floating_regs);
            fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
            fprintf(assembly_file_ptr, "\t\t\t\talign_16_rsp ;aligning at 16 byte boundary\n");
            fprintf(assembly_file_ptr, "\t\t\t\tcall printf\n");
            fprintf(assembly_file_ptr, "\t\t\t\tremove_align_16_rsp ;aligning at 16 byte boundary\n");
            fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str_no_new_line \" \"\n");
            fprintf(assembly_file_ptr, "\t\t\t\tsub RDX, %d ;move pointer to next elem of array\n", width);
            fprintf(assembly_file_ptr, "\t\t\t\tdec RCX ;decrement num of elem count to be scanned\n");
            fprintf(assembly_file_ptr, "\t\t\t\tjnz %s\n", quad.cnstrct_code_begin);            
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\"\n");
        }
        break;
        default:
        {
            fprintf(assembly_file_ptr, "\t\t\t\t		;type error, cannot take input \"%%d\" \n");
        }
        break;
    }    
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\" ;print a newline after output\n");
    
    
    /** 
     * ToDo:
     * There are unmatching push and pops coming in the generated code
     */
}

void assign_code_gen(quad_node quad){
    /**
     * @brief no special care required for assigning array to array
     * Simply copy value stored at second's location to first's location
     * take width number of bytes from source and copy to dest
     */
    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *res_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    char *arg1_str;
    
    arg1_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_arg1 = 0;
    
    if(arg1_type_ptr){
        offset_arg1 = arg1_type_ptr->offset_used;
        snprintf(arg1_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg1);
    }
    else{
        strncpy(arg1_str, quad.arg1, MAX_LEXEME_LEN);
    }

    int offset_result = res_type_ptr->offset_used;
    
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    if(res_type_ptr->name == REAL){
        fprintf(assembly_file_ptr, "\t\t\t\t;Assignment to a real\n");
        if(!arg1_type_ptr){
            char *real_str = newlabel();
            fprintf(assembly_file_ptr, "\t\t\t\tsection .data\n");
            fprintf(assembly_file_ptr, "\t\t\t\t%s: dq %s\n",real_str, arg1_str);
            fprintf(assembly_file_ptr, "\t\t\t\tsection .text\n");
            fprintf(assembly_file_ptr, "\t\t\t\tmovsd XMM0, [%s]\n", real_str);
        }
        else
            fprintf(assembly_file_ptr, "\t\t\t\tmovsd XMM0, %s\n", arg1_str);
        fprintf(assembly_file_ptr, "\t\t\t\tmovsd [RBP - %d], XMM0\n", offset_result);
    }
    else{
        fprintf(assembly_file_ptr, "\t\t\t\t;Assignment to a %s\n", terminal_string[res_type_ptr->name]);
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n", arg1_str);
        fprintf(assembly_file_ptr, "\t\t\t\tmov [RBP - %d], RAX\n", offset_result);
    }
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void jmp_if_true_code_gen(quad_node quad){
    /**
     * @brief True is represented as 1
     * So subtract one from the value
     * And jump if answer is zero(jz isntrn)
     */
    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);    
    
    char *arg1_str;
    
    arg1_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_arg1 = 0;
    
    if(arg1_type_ptr){
        offset_arg1 = arg1_type_ptr->offset_used;
        snprintf(arg1_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg1);
    }
    else{
        strncpy(arg1_str, quad.arg1, MAX_LEXEME_LEN);
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    fprintf(assembly_file_ptr, "\t\t\t\t;jmp if true code\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n", arg1_str);
    fprintf(assembly_file_ptr, "\t\t\t\tsub RAX, 1\n");
    fprintf(assembly_file_ptr, "\t\t\t\tjz %s\n", quad.arg2);
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void param_code_gen(quad_node quad){
    /**
     * @brief Push the parameter on the stack
     */
    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);

    int offset_arg1 = 0;
    offset_arg1 = arg1_type_ptr->offset_used;
    fprintf(assembly_file_ptr, "\t\t\t\t;pushing a parameter(%s) on stack\n", quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, RBP\n");
    fprintf(assembly_file_ptr, "\t\t\t\tsub RAX, %d\n", offset_arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tpush RAX\n");

    /**
     * @brief If type is array, push range values on stack
     */
    if(arg1_type_ptr->name == ARRAY){
        if(arg1_type_ptr->typeinfo.array.is_dynamic.range_high){
            char *range_high = arg1_type_ptr->typeinfo.array.range_high.lexeme;
            fprintf(assembly_file_ptr, "\t\t\t\t;get the value of array high range param %s\n", range_high);
            type *rhigh_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_high, quad.encl_fun_type_ptr, NULL);
            int rhigh_offset = rhigh_type->offset_used;
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d]\n", rhigh_offset);
        }
        else{
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d\n", arg1_type_ptr->typeinfo.array.range_high.value);
        }

        fprintf(assembly_file_ptr, "\t\t\t\t;Push the high range value on stack\n");
        fprintf(assembly_file_ptr, "\t\t\t\tpush RAX\n");

        if(arg1_type_ptr->typeinfo.array.is_dynamic.range_low){
            char *range_low = arg1_type_ptr->typeinfo.array.range_low.lexeme;
            fprintf(assembly_file_ptr, "\t\t\t\t;get the value of array low range param %s\n", range_low);
            type *rlow_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_low, quad.encl_fun_type_ptr, NULL);
            int rlow_offset = rlow_type->offset_used;
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d]\n", rlow_offset);
        }
        else{
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d\n", arg1_type_ptr->typeinfo.array.range_low.value);
        }

        fprintf(assembly_file_ptr, "\t\t\t\t;Push the low range value on stack\n");
        fprintf(assembly_file_ptr, "\t\t\t\tpush RAX\n");
    }
}

void index_copy_code_gen(quad_node quad){
    /**
     * @brief arr[i] := smth
     * get offset for arr[i]
     * store "smth" in the reg
     * then from reg to memory locn of arr[i]
     */
    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *arg2_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg2, quad.encl_fun_type_ptr, NULL);
    type *res_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    char *arg1_str, *arg2_str;
    
    arg1_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);
    arg2_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_arg1 = 0;
    
    if(arg1_type_ptr){
        offset_arg1 = arg1_type_ptr->offset_used;
        snprintf(arg1_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg1);
    }
    else{
        strncpy(arg1_str, quad.arg1, MAX_LEXEME_LEN);
    }
    
    int offset_arg2 = 0;
    
    if(arg2_type_ptr){
        offset_arg2 = arg2_type_ptr->offset_used;
        snprintf(arg2_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg2);
    }
    else{
        strncpy(arg2_str, quad.arg2, MAX_LEXEME_LEN);
    }
    
    int offset_result = res_type_ptr->offset_used;
    
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    fprintf(assembly_file_ptr, "\t\t\t\t; index copy\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d]\n", offset_result);
    fprintf(assembly_file_ptr, "\t\t\t\tsub RAX, %s\n", arg1_str);
    fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, %s\n", arg2_str);
    fprintf(assembly_file_ptr, "\t\t\t\tmov [RAX], RBX\n");
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void array_access_code_gen(quad_node quad){
    /**
     * @brief smth := arr[i]
     * get offset for arr[i]
     * Store value from there to register
     * from register to locn of "smth"
     */
    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    type *arg2_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg2, quad.encl_fun_type_ptr, NULL);
    type *res_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.result, quad.encl_fun_type_ptr, NULL);
    
    char *arg2_str;

    arg2_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_arg1 = 0;
    
    offset_arg1 = arg1_type_ptr->offset_used;
    
    int offset_arg2 = 0;
    
    if(arg2_type_ptr){
        offset_arg2 = arg2_type_ptr->offset_used;
        snprintf(arg2_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg2);
    }
    else{
        strncpy(arg2_str, quad.arg2, MAX_LEXEME_LEN);
    }
    
    int offset_result = res_type_ptr->offset_used;

    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    fprintf(assembly_file_ptr, "\t\t\t\t; array access\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov RCX, %s\n", arg2_str);
    fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, [RBP - %d]\n", offset_arg1);   // pick up BA of array in RBX
    fprintf(assembly_file_ptr, "\t\t\t\tsub RBX, RCX\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBX]\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov [RBP - %d], RAX\n", offset_result);
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void dynamic_arr_space_code_gen(quad_node quad){
    /**
     * @brief Reserve somehow the n-m+1 * width_of_elem space
     * For this dynamic array
     */
    fprintf(assembly_file_ptr, "\t\t\t\t;Dynamic array %s's declaration\n", quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\t;push_all ;don't push bcz RSP value has to be changed to allocate space\n");
    type *var_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    
    char *range_low, *range_high;
    int rlow_val = -1, rhigh_val = -1;
    int width = WIDTH_POINTER;
    int offset = var_type_ptr->offset_used;

    if(var_type_ptr->typeinfo.array.is_dynamic.range_low){
        range_low = var_type_ptr->typeinfo.array.range_low.lexeme;
        type *rlow_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_low, quad.encl_fun_type_ptr, NULL);
        int offset = rlow_type->offset_used;
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d] ; Load low range value from %s's location\n", offset, range_low);
    }
    else{
        rlow_val = var_type_ptr->typeinfo.array.range_low.value;
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d ; Load low range value into RAX\n", rlow_val);                
    }

    if(var_type_ptr->typeinfo.array.is_dynamic.range_high){
        range_high = var_type_ptr->typeinfo.array.range_high.lexeme;
        type *rhigh_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_high, quad.encl_fun_type_ptr, NULL);
        int offset = rhigh_type->offset_used;
        fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, [RBP - %d] ; Load high range value from %s's location\n", offset, range_high);
    }
    else{
        rhigh_val = var_type_ptr->typeinfo.array.range_high.value;
        fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, %d ; Load high range value into RBX\n", rhigh_val);                
    }

    fprintf(assembly_file_ptr, "\t\t\t\t;Store count of array elements in RCX\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov RCX, RBX\n");
    fprintf(assembly_file_ptr, "\t\t\t\tcmp ECX, EAX\n");
    char *range_validity_chk_lbl = newlabel();
    fprintf(assembly_file_ptr, "\t\t\t\tjge %s\n", range_validity_chk_lbl);
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Low range must be <= high range, exiting program\"\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, 0\n");
    fprintf(assembly_file_ptr, "\t\t\t\tcall exit\n");
    fprintf(assembly_file_ptr, "\t\t\t\t%s:\n", range_validity_chk_lbl);
    fprintf(assembly_file_ptr, "\t\t\t\tsub RCX, RAX\n");
    fprintf(assembly_file_ptr, "\t\t\t\tadd RCX, 1  ; count = high_range - low_range + 1\n");

    fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d\n", WIDTH_POINTER);
    fprintf(assembly_file_ptr, "\t\t\t\tmul RCX;RCX = count_elems * width\n");
    
    fprintf(assembly_file_ptr, "\t\t\t\t;Save current RSP(location of alocation of elements of array) at array's name location\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov QWORD [RBP - %d], RSP\n", offset);

    fprintf(assembly_file_ptr, "\t\t\t\t;Allocate space on stack's top for elems of array and assign address to array's pointer variable {RSP -= count*width}\n");
    fprintf(assembly_file_ptr, "\t\t\t\tsub RSP, RAX ;RAX because result of multiplication goes to RDX : RAX\n");
    fprintf(assembly_file_ptr, "\t\t\t\t;Assuming result won't cross 64 bits\n");

    fprintf(assembly_file_ptr, "\t\t\t\t;pop_all ; not pushed, so no pop too\n");
}

void static_arr_space_code_gen(quad_node quad){
    /**
     * Space is already allocated, just store base address of first element at location of base address
     * MOV [RBP + arr's offset], RBP + arr's offset + width_ptr
     */
    type *arr_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    int offset = arr_type_ptr->offset_used;
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    fprintf(assembly_file_ptr, "\t\t\t\t;Declaring static array %s, store %s[0]'s address at location of %s\n", quad.arg1, quad.arg1, quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tMOV RAX, RBP\n");
    fprintf(assembly_file_ptr, "\t\t\t\tSUB RAX, %d\n", offset);
    fprintf(assembly_file_ptr, "\t\t\t\tMOV RBX, RAX\n");
    fprintf(assembly_file_ptr, "\t\t\t\tSUB RAX, %d\n", WIDTH_POINTER);
    fprintf(assembly_file_ptr, "\t\t\t\tMOV [RBX], RAX\n");
    fprintf(assembly_file_ptr, "\t\t\t\t;Declaration ends\n");
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void fn_space_code_gen(quad_node quad){
    /**
     * @brief Reserve number of bytes equal to offset value
     * of this function, Also output function's label
     */

    fprintf(assembly_file_ptr, "%s:\n", quad.arg1);
    
    int num_elems = 0;
    type *type_ptr;
    int offset = 0;
    st_wrapper *table_ptr = quad.curr_scope_table_ptr, *lvl_start_ptr;

    for(int i=0; i < HASH_SIZE; i++){                        
        type_ptr = (type*)(table_ptr->table[i].value);
        if(type_ptr != NULL){
            // printf("adding2 offset for %s\n", table_ptr->table[i].lexeme);
            // printf("while gen space for fn, lexeme in ht %llu found to be %s\n",table_ptr,table_ptr->table[i].lexeme );
            num_elems++;
            offset += WIDTH_POINTER;
            if(type_ptr->name == ARRAY){
                if(type_ptr->typeinfo.array.is_dynamic.range_high == false && type_ptr->typeinfo.array.is_dynamic.range_low == false){
                    offset += (type_ptr->typeinfo.array.range_high.value - type_ptr->typeinfo.array.range_low.value + 1) * WIDTH_POINTER;
                }
            }
        }            
    }

    lvl_start_ptr = table_ptr->leftmost_child_table;
    while(lvl_start_ptr != NULL){
        table_ptr = lvl_start_ptr;
        while(table_ptr){
            for(int i=0; i < HASH_SIZE; i++){                        
                type_ptr = (type*)(table_ptr->table[i].value);
                if(type_ptr != NULL){
                    // printf("adding2 offset for %s\n", table_ptr->table[i].lexeme);
                    // printf("while gen space for fn, lexeme in ht %llu found to be %s\n",table_ptr,table_ptr->table[i].lexeme );
                    num_elems++;
                    offset += WIDTH_POINTER;
                    if(type_ptr->name == ARRAY){
                        if(type_ptr->typeinfo.array.is_dynamic.range_high == false && type_ptr->typeinfo.array.is_dynamic.range_low == false){
                            offset += (type_ptr->typeinfo.array.range_high.value - type_ptr->typeinfo.array.range_low.value + 1) * WIDTH_POINTER;
                        }
                    }
                }            
            }
            table_ptr = table_ptr->sibling_table;
        }
        lvl_start_ptr = lvl_start_ptr->leftmost_child_table;
    }

    // printf("total_num_elems : %d\n", num_elems);

    fprintf(assembly_file_ptr, "\t\t\t\tENTER %d, 0\n", offset);
    fprintf(assembly_file_ptr, "\t\t\t\t;reserve space for the input/output params of fn, later flush this space\n");
    /**
     * @brief Insert the parameters in current scope's table too
     * This is to enable updation locally in function
     * While returning output params 
     */
    
    int total_offset_reqd = 0;

    type *fn_type_ptr = (type*)key_search_recursive(root_sym_tab_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);

    params_list_node *inp_param = NULL;
    int inp_param_num = 0;
    if(fn_type_ptr && fn_type_ptr->typeinfo.module.input_params){
        inp_param = fn_type_ptr->typeinfo.module.input_params->first;
        total_offset_reqd += fn_type_ptr->typeinfo.module.input_params->length * WIDTH_POINTER;
        if(inp_param->t->name == ARRAY)
            total_offset_reqd += 2 * WIDTH_POINTER;     // for range values
    }

    params_list_node *outp_param = NULL;
    int outp_param_num = 0;
    if(fn_type_ptr && fn_type_ptr->typeinfo.module.output_params){
        outp_param = fn_type_ptr->typeinfo.module.output_params->first;
        total_offset_reqd += fn_type_ptr->typeinfo.module.output_params->length * WIDTH_POINTER;
    }

    fprintf(assembly_file_ptr, "\t\t\t\tsub RSP, %d; allocated space for i/o variables on stack\n", total_offset_reqd);

    while(inp_param){
        
        char *param_str = inp_param->param_name;
        
        type *param_type  = inp_param->t;

        /**
         * @brief Check if param type is dynamic array, copy the range values from stack
         * And if static, do range checking
         */

        if(param_type->name == ARRAY){

            if(param_type->typeinfo.array.is_dynamic.range_low){
            //    param_type->typeinfo.array.is_dynamic.range_low = false;
                // param_type->typeinfo.array.range_low.value = 
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP + 16 + 8 + 8*%d] ;take range_low value coming from caller\n", inp_param_num);
                int offset_rlow;
                type *rlow_entry = (type*)key_search_recursive(quad.curr_scope_table_ptr, param_type->typeinfo.array.range_low.lexeme,
                        quad.encl_fun_type_ptr, NULL);
                offset_rlow = rlow_entry->offset_used;
                fprintf(assembly_file_ptr, "\t\t\t\tmov [RBP - %d], RAX ;copy it to my local location\n", offset_rlow);
                inp_param_num++;
            }
            else{
                char *rlow_match_label = newlabel();
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP + 16 + 8 + 8*%d] ;take range_low value coming from caller\n", inp_param_num);
                fprintf(assembly_file_ptr, "\t\t\t\t;Check if this value is not equal to that stored in symbol table\n");
                fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, %d\n\
                cmp RAX, RBX \n\
                jz %s; indices matched, continue normal execution\n\
                print_str \"Indices in function calling do not match at low index, exiting program\"\n\
                mov RDI, 0\n\
                call exit\n\
%s:\n", param_type->typeinfo.array.range_low.value, rlow_match_label, rlow_match_label);
                inp_param_num++;
            }

            if(param_type->typeinfo.array.is_dynamic.range_high){
            //    param_type->typeinfo.array.is_dynamic.range_high = false;
                // param_type->typeinfo.array.range_high.value = 
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP + 16 + 8 + 8*%d] ;take range_high value coming from caller\n", inp_param_num);
                int offset_rhigh;
                type *rhigh_entry = (type*)key_search_recursive(quad.curr_scope_table_ptr, param_type->typeinfo.array.range_high.lexeme,
                        quad.encl_fun_type_ptr, NULL);
                offset_rhigh = rhigh_entry->offset_used;
                fprintf(assembly_file_ptr, "\t\t\t\tmov [RBP - %d], RAX ;copy it to my local location\n", offset_rhigh);
                inp_param_num++;
            }
            else{
                char *rhigh_match_label = newlabel();
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP + 16 + 8 + 8*%d] ;take range_high value coming from caller\n", inp_param_num);
                fprintf(assembly_file_ptr, "\t\t\t\t;Check if this value is not equal to that stored in symbol table\n");
                fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, %d\n\
                cmp RAX, RBX \n\
                jz %s; indices matched, continue normal execution\n\
                print_str \"Indices in function calling do not match at high index, exiting program\"\n\
                mov RDI, 0\n\
                call exit\n\
%s:\n", param_type->typeinfo.array.range_high.value, rhigh_match_label, rhigh_match_label);
                inp_param_num++;
            }

        }



        param_type->offset = quad.encl_fun_type_ptr->typeinfo.module.curr_offset;
        param_type->offset_used = quad.encl_fun_type_ptr->typeinfo.module.curr_offset_used;
        // printf("Assigned offset %d to a param %s\n", param_type->offset_used, param_str);
        quad.encl_fun_type_ptr->typeinfo.module.curr_offset += param_type->width;
        quad.encl_fun_type_ptr->typeinfo.module.curr_offset_used += WIDTH_POINTER;

        if(param_type->name == ARRAY){
            quad.encl_fun_type_ptr->typeinfo.module.curr_offset += (WIDTH_INTEGER*2);
            quad.encl_fun_type_ptr->typeinfo.module.curr_offset_used += 2*WIDTH_POINTER;            
        }

        hash_insert_ptr_val(quad.curr_scope_table_ptr->table, param_str, param_type);

        fprintf(assembly_file_ptr, "\t\t\t\t; Allocating space to %s inp param on stack\n", inp_param->param_name);
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP + 16 + 8 + 8*%d]; copy value of this param from caller\n", inp_param_num);
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RAX] ;copy to my local space for the param\n");
        fprintf(assembly_file_ptr, "\t\t\t\tmov [RBP - %d], RAX ;copy to my local space for the param\n", param_type->offset_used);

        inp_param_num++;
        inp_param = inp_param->next;
    }   

    while(outp_param){
        
        char *param_str = outp_param->param_name;
        
        type *param_type  = outp_param->t;
        param_type->offset = quad.encl_fun_type_ptr->typeinfo.module.curr_offset;
        param_type->offset_used = quad.encl_fun_type_ptr->typeinfo.module.curr_offset_used;
        quad.encl_fun_type_ptr->typeinfo.module.curr_offset += param_type->width;
        quad.encl_fun_type_ptr->typeinfo.module.curr_offset_used += WIDTH_POINTER;
        hash_insert_ptr_val(quad.curr_scope_table_ptr->table, param_str, param_type);
        outp_param_num++;
        outp_param = outp_param->next;
    }    
}

void call_code_gen(quad_node quad){    
    fprintf(assembly_file_ptr, "\t\t\t\tpush RBP\n");
    fprintf(assembly_file_ptr, "\t\t\t\t%s %s\n", tac_op_str[quad.op], quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tpop RBP\n");
    type *calle_type = (type *)key_search_recursive(root_sym_tab_ptr, quad.arg1, NULL, NULL);
    
    params_list_node *inp_param = NULL;
    
    if(calle_type->typeinfo.module.input_params)
        inp_param = calle_type->typeinfo.module.input_params->first;
    
    fprintf(assembly_file_ptr, "\t\t\t\t;popping back input parameters\n");
    
    while(inp_param){
        fprintf(assembly_file_ptr, "\t\t\t\tpop RAX\n");
        inp_param = inp_param->next;
    }
    
    params_list_node *outp_param = NULL;
    if(calle_type->typeinfo.module.output_params)
        outp_param = calle_type->typeinfo.module.output_params->first;
    
    stack *stk_ptr = stack_init();

    while(outp_param){
        push(stk_ptr, outp_param);
        outp_param = outp_param->next;
    }

    fprintf(assembly_file_ptr, "\t\t\t\t;popping back output parameters\n");
    
    int outp_param_offset_caller = -1;
    type *outp_param_type_caller;
    
    while( top(stk_ptr) != NULL ){
        // outp_param = (params_list_node *)pop(stk_ptr);
        // printf("outp_param is %s\n", outp_param->param_name);
        // outp_param_type_caller = (type *)key_search_recursive(quad.curr_scope_table_ptr, outp_param->param_name, quad.encl_fun_type_ptr, NULL);
        // outp_param_offset_caller = outp_param_type_caller->offset_used;

        fprintf(assembly_file_ptr, "\t\t\t\tpop RAX\n");
        // fprintf(assembly_file_ptr, "\t\t\t\t;update value of %s in caller's record too\n", outp_param->param_name);
        // fprintf(assembly_file_ptr, "\t\t\t\tmov [RBP - %d], RAX\n", outp_param_offset_caller);
        // outp_param = outp_param->next;
        pop(stk_ptr);
    }
}

void return_code_gen(quad_node quad){

    type *fn_type_ptr = quad.encl_fun_type_ptr;

    params_list_node *outp_param = NULL;
    int total_offset_reqd = 0;
    int extra_inp_param_num = 0;    // will be 2 when inp param is of array type

    params_list_node *inp_param = NULL;
    if(fn_type_ptr && fn_type_ptr->typeinfo.module.input_params)
        inp_param = fn_type_ptr->typeinfo.module.input_params->first;

    while(inp_param){
        type *param_type  = inp_param->t;
        total_offset_reqd += WIDTH_POINTER;
        inp_param = inp_param->next;
        if(param_type->name == ARRAY)
            extra_inp_param_num += 2;
    }

    if(fn_type_ptr && fn_type_ptr->typeinfo.module.output_params)
        outp_param = fn_type_ptr->typeinfo.module.output_params->first;

    while(outp_param){        
        type *param_type  = outp_param->t;
        total_offset_reqd += WIDTH_POINTER;
        outp_param = outp_param->next;
    }


    if(fn_type_ptr && strcmp(fn_type_ptr->typeinfo.module.module_name, "main") != 0){
        if(fn_type_ptr->typeinfo.module.output_params)
            outp_param = fn_type_ptr->typeinfo.module.output_params->first;
    
        int num_inp_params = fn_type_ptr->typeinfo.module.input_params->length;    
        int outp_param_num = 0;
        int outp_param_offset_callee;
        type *outp_param_type_ptr_callee = NULL;
        fprintf(assembly_file_ptr, "\t\t\t\t; copy values to output parameters that u have computed so far\n");
        while(outp_param){
            outp_param_type_ptr_callee = (type *)key_search_recursive(quad.curr_scope_table_ptr, outp_param->param_name, quad.encl_fun_type_ptr, NULL);
            outp_param_offset_callee = outp_param_type_ptr_callee->offset_used;
            fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d]\n", outp_param_offset_callee);
            fprintf(assembly_file_ptr, "\t\t\t\tmov RBX, [RBP + 16 + 8 + 8*%d + 8*%d]\n", num_inp_params + extra_inp_param_num,outp_param_num);
            fprintf(assembly_file_ptr, "\t\t\t\tmov [RBX], RAX\n");
            outp_param_num++;
            outp_param = outp_param->next;
        }
    }    
    fprintf(assembly_file_ptr, "\t\t\t\t;Deallocate space given to I/O variables on stack\n");
    fprintf(assembly_file_ptr, "\t\t\t\tadd RSP, %d\n", total_offset_reqd);
    fprintf(assembly_file_ptr, "\t\t\t\tLEAVE\n");
    int offset = quad.encl_fun_type_ptr->typeinfo.module.curr_offset;
    fprintf(assembly_file_ptr, "\t\t\t\t%s\n\n", tac_op_str[quad.op]);
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
    type *arg1_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);    
    
    char *arg1_str;
    
    arg1_str = (char*)malloc(sizeof(char) * MAX_LEXEME_LEN);

    int offset_arg1 = 0;
    
    if(arg1_type_ptr){
        offset_arg1 = arg1_type_ptr->offset_used;
        snprintf(arg1_str, MAX_LEXEME_LEN, "[RBP - %d]", offset_arg1);
    }
    else{
        strncpy(arg1_str, quad.arg1, MAX_LEXEME_LEN);
    }

    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    fprintf(assembly_file_ptr, "\t\t\t\t;exit if condition is true\n");
    fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %s\n", arg1_str);
    fprintf(assembly_file_ptr, "\t\t\t\tsub RAX, 1\n");
    fprintf(assembly_file_ptr, "\t\t\t\tjnz %s\n", quad.cnstrct_code_begin);
    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, 0\n");
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Invalid array access, exiting program\"\n");
    fprintf(assembly_file_ptr, "\t\t\t\tcall exit\n");    
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
}

void generate_code(){    
    // printf("\nsizeof(int) = %lu\n", sizeof(int));
    fprintf(assembly_file_ptr, ";/***************************************\n\
                ;|GROUP-09|    \n\
    ;Aditya Upadhyay      -   2017A7PS0083P\n\
    ;Harpider Jot Singh   -   2017A7PS0057P\n\
    ;Jaladi Lakshmi Teja  -   2017A7PS0068P\n\
    ;Vishal Mittal        -   2017A7PS0080P\n\
    ;Yash Vijay           -   2017A7PS0072P\n\
;*****************************************\n");
    fprintf(assembly_file_ptr, "extern printf, scanf, exit\n");
    fprintf(assembly_file_ptr, "section .data\n");
    fprintf(assembly_file_ptr, "\t\tffour_reg_fmt: db `RAX = %%ld, RBX = %%ld, RCX = %%ld, RDX = %%ld`, 10, 0\n");
    fprintf(assembly_file_ptr, "\t\tlfour_reg_fmt: db `RSP = %%ld, RBP = %%ld, RSI = %%ld, RDI = %%ld\\n`, 10, 0\n");
    fprintf(assembly_file_ptr, "\t\tint_fmt: db \"%%d\", 0\n");
    fprintf(assembly_file_ptr, "\t\treal_fmt: db \"%%lf\", 0\n");
    fprintf(assembly_file_ptr, "\t\tbool_fmt: db \"%%d\", 0\n");
    fprintf(assembly_file_ptr, "\t\tstr_fmt: db \"%%s\",10, 0\n");
    fprintf(assembly_file_ptr, "\t\tstr_no_new_line_fmt: db \"%%s\",0\n");    
    
    fprintf(assembly_file_ptr, "section .text\n");
    fprintf(assembly_file_ptr, "\t\t\t\t; Push all registers on stack to prevent value loss\n");
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
    
    fprintf(assembly_file_ptr, "\t\t\t\t; Restore values of registers from stack\n");
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

    fprintf(assembly_file_ptr, "\t\t\t\t; Print RAX, RBX, RCX, RDX values\n");
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
                    align_16_rsp ;align RSP to 16 byte boundary\n\
                    call printf \n\
                    remove_align_16_rsp ;realign RSP\n\
                    pop_all\n\
                %%endmacro \n\n");
    
    fprintf(assembly_file_ptr, "\t\t\t\t; Print RSP, RBP, RSI, RDI values\n");
    fprintf(assembly_file_ptr,
            "\t\t\t\t%%macro print_last_four 0 \n\
                    push_all\n\
                    push RDI \n\
                    push RSI \n\
                    push RBP \n\
                    push RSP \n\
                    mov RDI, lfour_reg_fmt		; first arg, format \n\
                    pop RSI \n\
                    add RSI, 88 ; 88 because RSP has pushed additional 11 registers(8 in push_all, and 4 after that), so original value is 88 more than current\n\
                    pop RDX \n\
                    pop RCX \n\
                    pop R8 \n\
                    mov RAX, 0 \n\
                    align_16_rsp ;align RSP to 16 byte boundary\n\
                    call printf \n\
                    remove_align_16_rsp ;realign RSP\n\
                    pop_all\n\
                %%endmacro \n\n");

    fprintf(assembly_file_ptr,
            "\t\t\t\t%%macro print_str 1 \n\
                    section .data\n\
                        %%%%str db %%1,0 ;%% is to make label local to this macro, to enable mutiple calls within same label\n\
                    section .text\n\
                        push_all\n\
                        mov RDI, str_fmt    ;\"%%s\\n\"\n\
                        mov RSI, %%%%str\n\
                        mov RAX, 0 \n\
                        align_16_rsp ;align RSP to 16 byte boundary\n\
                        call printf \n\
                        remove_align_16_rsp ;realign RSP\n\
                        pop_all\n\
                %%endmacro \n\n");
    fprintf(assembly_file_ptr,
            "\t\t\t\t%%macro print_str_no_new_line 1 \n\
                    section .data\n\
                        %%%%str db %%1,0 ;%% is to make label local to this macro, to enable mutiple calls within same label\n\
                    section .text\n\
                        push_all\n\
                        mov RDI, str_no_new_line_fmt    ;\"%%s\"\n\
                        mov RSI, %%%%str\n\
                        mov RAX, 0 \n\
                        align_16_rsp ;align RSP to 16 byte boundary\n\
                        call printf \n\
                        remove_align_16_rsp ;realign RSP\n\
                        pop_all\n\
                %%endmacro \n\n");

    fprintf(assembly_file_ptr, 
            "\t\t\t\t%%macro align_16_rsp 0 \n\
                    push RBX\n\
					push RDX\n\
					push RAX\n\
					mov RDX, 0\n\
					mov RAX, RSP\n\
					mov RBX, 16\n\
					idiv RBX\n\
					mov RBX, RDX\n\
					pop RAX\n\
					pop RDX\n\
					sub RSP, RBX\n\
                %%endmacro \n\n");
    
    fprintf(assembly_file_ptr, 
            "\t\t\t\t%%macro remove_align_16_rsp 0 \n\
                    add RSP, RBX\n\
					pop RBX\n\
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
                call_code_gen(quadruples[i]);
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
                return_code_gen(quadruples[i]);
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