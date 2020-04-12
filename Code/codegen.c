#include "codegen.h"
#include <malloc.h>
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
                call scanf \n", offset);
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
    int width;

    fprintf(assembly_file_ptr, "\t\t\t\t; Code to take input value(s) of %s\n", quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    switch(var_type_ptr->name){
        case INTEGER:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Input: Enter an integer value\"\n");
            one_input_code_gen(INTEGER, offset);
        }
        break;
        case REAL:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Input: Enter a real value\"\n");
            one_input_code_gen(REAL, offset);
        }
        break;
        case BOOLEAN:
        {
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Input: Enter a boolean value\"\n");
            one_input_code_gen(BOOLEAN, offset);
        }
        break;
        case ARRAY:
        {
            int num_floating_regs = 0;
            /**
             * @brief Code for format of input == base type
             */
            
            switch(var_type_ptr->typeinfo.array.primitive_type){
                case INTEGER:
                {
                    width = WIDTH_INTEGER;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
                }
                break;
                case REAL:
                {
                    width = WIDTH_REAL;
                    num_floating_regs = 1;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, real_fmt		;first arg, format \"%%d\" \n");
                }
                break;
                case BOOLEAN:
                {
                    width = WIDTH_BOOLEAN;
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
                int offset = rlow_type->offset;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d] ; Load low range value from %s's location\n", offset, range_low);
            }
            else{
                rlow_val = var_type_ptr->typeinfo.array.range_low.value;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d ; Load low range value into RAX\n", rlow_val);                
            }

            if(var_type_ptr->typeinfo.array.is_dynamic.range_high){
                range_high = var_type_ptr->typeinfo.array.range_high.lexeme;
                type *rhigh_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_high, quad.encl_fun_type_ptr, NULL);
                int offset = rhigh_type->offset;
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
            fprintf(assembly_file_ptr, "\t\t\t\t;just check RCX value to verify\n");            
            print_regs_code_gen();
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Input: Enter {RCX} number of aray elements\"\n");

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

void one_output_code_gen(token_name type_name, int offset){
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    switch(type_name){
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
                movq XMM0, [RDX]  ; for real, value stored at offset should be passed to printf\n\
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
                mov RSI, [RDX]    \n\
                mov RAX, 0 \n\
                call printf \n\
                pop_all\n", offset);
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
    print_a_type(var_type_ptr);
    int offset = var_type_ptr->offset;
    int width;
    fprintf(assembly_file_ptr, "\t\t\t\t; Code to output value(s) of %s\n", quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Output: \"\n");
    fprintf(assembly_file_ptr, "\t\t\t\tpush_all\n");
    switch(var_type_ptr->name){
        case INTEGER:
        {
            one_output_code_gen(INTEGER, offset);
        }
        break;
        case REAL:
        {
            one_output_code_gen(REAL, offset);
        }
        break;
        case BOOLEAN:
        {
            one_output_code_gen(BOOLEAN, offset);
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
                    width = WIDTH_INTEGER;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, int_fmt		;first arg, format \"%%d\" \n");
                }
                break;
                case REAL:
                {
                    width = WIDTH_REAL;
                    num_floating_regs = 1;
                    fprintf(assembly_file_ptr, "\t\t\t\tmov RDI, real_fmt		;first arg, format \"%%d\" \n");
                }
                break;
                case BOOLEAN:
                {
                    width = WIDTH_BOOLEAN;
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
                int offset = rlow_type->offset;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d] ; Load low range value from %s's location\n", offset, range_low);
            }
            else{
                rlow_val = var_type_ptr->typeinfo.array.range_low.value;
                fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d ; Load low range value into RAX\n", rlow_val);                
            }

            if(var_type_ptr->typeinfo.array.is_dynamic.range_high){
                range_high = var_type_ptr->typeinfo.array.range_high.lexeme;
                type *rhigh_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_high, quad.encl_fun_type_ptr, NULL);
                int offset = rhigh_type->offset;
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
            fprintf(assembly_file_ptr, "\t\t\t\t;just check RCX value to verify\n");            
            print_regs_code_gen();
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Output:\"\n");
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\"\n");

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
            fprintf(assembly_file_ptr, "\t\t\t\tcall printf\n");
            fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
            fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\"\n");
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
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"\" ;print a newline after output\n");
    fprintf(assembly_file_ptr, "\t\t\t\tpop_all\n");
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
    fprintf(assembly_file_ptr, "\t\t\t\t;Dynamic array %s's declaration\n", quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\t;push_all ;don't push bcz RSP value has to be changed to allocate space\n");
    type *var_type_ptr = (type*)key_search_recursive(quad.curr_scope_table_ptr, quad.arg1, quad.encl_fun_type_ptr, NULL);
    
    char *range_low, *range_high;
    int rlow_val = -1, rhigh_val = -1;
    int width = var_type_ptr->width;
    int offset = var_type_ptr->offset;

    if(var_type_ptr->typeinfo.array.is_dynamic.range_low){
        range_low = var_type_ptr->typeinfo.array.range_low.lexeme;
        type *rlow_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_low, quad.encl_fun_type_ptr, NULL);
        int offset = rlow_type->offset;
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, [RBP - %d] ; Load low range value from %s's location\n", offset, range_low);
    }
    else{
        rlow_val = var_type_ptr->typeinfo.array.range_low.value;
        fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d ; Load low range value into RAX\n", rlow_val);                
    }

    if(var_type_ptr->typeinfo.array.is_dynamic.range_high){
        range_high = var_type_ptr->typeinfo.array.range_high.lexeme;
        type *rhigh_type = (type*)key_search_recursive(quad.curr_scope_table_ptr, range_high, quad.encl_fun_type_ptr, NULL);
        int offset = rhigh_type->offset;
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
    fprintf(assembly_file_ptr, "\t\t\t\t;just check RCX value to verify\n");
    print_regs_code_gen();
    
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str \"Declared dynamic array with RCX elements\"\n");
    fprintf(assembly_file_ptr, "\t\t\t\tprint_str\"\"\n");

    fprintf(assembly_file_ptr, "\t\t\t\tmov RAX, %d\n", width);
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
    int offset = arr_type_ptr->offset;
    fprintf(assembly_file_ptr, "\t\t\t\t;Declaring static array %s, store %s[0]'s address at location of %s\n", quad.arg1, quad.arg1, quad.arg1);
    fprintf(assembly_file_ptr, "\t\t\t\tMOV RAX, RBP\n");
    fprintf(assembly_file_ptr, "\t\t\t\tSUB RAX, %d\n", offset);
    fprintf(assembly_file_ptr, "\t\t\t\tMOV RBX, RAX\n");
    fprintf(assembly_file_ptr, "\t\t\t\tSUB RAX, %d\n", WIDTH_POINTER);
    fprintf(assembly_file_ptr, "\t\t\t\tMOV [RBX], RAX\n");
    fprintf(assembly_file_ptr, "\t\t\t\t;Declaration ends\n");
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
    // printf("\nsizeof(int) = %lu\n", sizeof(int));
    fprintf(assembly_file_ptr, "extern printf, scanf\n");
    fprintf(assembly_file_ptr, "section .data\n");
    fprintf(assembly_file_ptr, "\t\tffour_reg_fmt: db `RAX = %%ld, RBX = %%ld, RCX = %%ld, RDX = %%ld`, 10, 0\n");
    fprintf(assembly_file_ptr, "\t\tlfour_reg_fmt: db `RSP = %%ld, RBP = %%ld, RSI = %%ld, RDI = %%ld\\n`, 10, 0\n");
    fprintf(assembly_file_ptr, "\t\tint_fmt: db \"%%d\", 0\n");
    fprintf(assembly_file_ptr, "\t\treal_fmt: db \"%%lf\", 0\n");
    fprintf(assembly_file_ptr, "\t\tbool_fmt: db \"%%d\", 0\n");
    fprintf(assembly_file_ptr, "\t\tstr_fmt: db \"%%s\", 0\n");
    
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
                    call printf \n\
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
                    call printf \n\
                    pop_all\n\
                %%endmacro \n\n");

    fprintf(assembly_file_ptr,
            "\t\t\t\t%%macro print_str 1 \n\
                    section .data\n\
                        %%%%str db %%1,10,0 ;%% is to make label local to this macro, to enable mutiple calls within same label\n\
                    section .text\n\
                        push_all\n\
                        mov RDI, str_fmt    ;\"%%s\"\n\
                        mov RSI, %%%%str\n\
                        mov RAX, 0 \n\
                        call printf \n\
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
                print_regs_code_gen();
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