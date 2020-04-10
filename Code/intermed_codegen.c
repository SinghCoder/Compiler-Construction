#include "intermed_codegen.h"
#include <string.h>
#include <stdlib.h>
#include "stackADT.h"

void assign_next_label(tree_node *ast_node)
{
    if(ast_node->sibling){
        ast_node->label.next_label = newlabel();                            
        // code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL);
    }
    else{
        ast_node->label.next_label = ast_node->parent->label.next_label;        
        if(ast_node->label.next_label == NULL){
            char *fun_name = ast_node->encl_fun_type_ptr->typeinfo.module.module_name;
            char *fn_end_str = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
            strcpy(fn_end_str, fun_name);
            strcat(fn_end_str, "_end");
            ast_node->label.next_label = fn_end_str;
        }
            
    }
    // printf("Next label is assigned as %s\n", ast_node->label.next_label);
}

void print_quadruples()
{
    printf("%-12s | ","operator");
    printf("%-12s | ","arg1");
    printf("%-12s | ","arg2");
    printf("%-12s | ","result");
    printf("%-12s | ","level_no");
    printf("%-50s | ","Result type");
    printf("\n");
    printf("%-12s | ","-----------");
    printf("%-12s | ","-----------");
    printf("%-12s | ","-----------");
    printf("%-12s | ","-----------");
    printf("%-12s | ","-----------");
    printf("%-50s | ","-----------");
    printf("\n\n");

    for(int i = 0; i < quad_count; i++){
        printf("%-12s | ", tac_op_str[quadruples[i].op] );
        if(quadruples[i].arg1){
            printf("%-12s | ", quadruples[i].arg1);
        }
        else
            printf("%-12s | ","(empty)");
        
        if(quadruples[i].arg2){
            printf("%-12s | ", quadruples[i].arg2);
        }
        else
            printf("%-12s | ","(empty)");
        
        if(quadruples[i].result){
            printf("%-12s | ", quadruples[i].result);
        }
        else
            printf("%-12s | ","(empty)");
        
        if(quadruples[i].curr_scope_table_ptr){
            printf("%-12d | ",quadruples[i].curr_scope_table_ptr->level_num);
            if(quadruples[i].result){
                type *res_type = (type *)key_search_recursive(quadruples[i].curr_scope_table_ptr, quadruples[i].result, quadruples[i].encl_fun_type_ptr, NULL);
                print_a_type(res_type);
            }
        }            
        else
            printf("%-12s | ", "Sym tab NULL");
        printf("\n");
    }
}

void intermed_codegen_init()
{
    label_count = 0;
    temp_count = 0;
    quad_count = 0;
    switch_tbl_entry_num = 0;
    char *tac_op_str_tmp[NUM_TAC_OP] = {"+", "-", "*", "/", ">=", ">", "<=", "<", "==", "!=", "&&",
                                        "||","u_plus", "u_minus", "label","input", "output", "assign", "jmp", "jmp_if_true",
        "inp_param" ,"outp_param", "call", "indexed_copy", "array_access", "dyn_arr_dec","stat_arr_dec", "ret", "function", "exit_if_true"};
    for(int i=0; i<NUM_TAC_OP; i++){        
        strcpy(tac_op_str[i], tac_op_str_tmp[i]);
    }

    to_be_attached_label = NULL;
}

char *newlabel()
{
    char *label = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
    sprintf(label, "label_%d", label_count);
    // printf("Emitted label %s\n", label);
    label_count++;
    return label;
}

tree_node *newtemp(tree_node *expr1_node, operator op, tree_node *expr2_node, token_name type_name)
{
    // printf("Called newtemp()\n");
    tree_node *tmp_node = create_tree_node();        
    char *tmp_str = (char*)malloc(sizeof(char) * MAX_TEMP_LEN);
    sprintf(tmp_str, "temp_%d", temp_count);
    temp_count++;
    
    tmp_node->addr = tmp_str;
    tmp_node->encl_fun_type_ptr = expr1_node->encl_fun_type_ptr;
    tmp_node->scope_sym_tab = expr1_node->scope_sym_tab;
    tmp_node->sym.is_terminal = true;
    tmp_node->sym.t = ID;
    strcpy( tmp_node->token.id.str, tmp_str);
    tmp_node->token.name = ID;
    
    type *tmp_type, *e1_type, *e2_type, *e_type;
    if(expr1_node && type_name == TYPE_ERROR){
        
        e1_type = create_type();        
        *e1_type = get_expr_type(expr1_node, expr1_node->scope_sym_tab);
        
        if(expr2_node != NULL){
            e2_type = create_type();
            e_type = create_type();
            *e2_type = get_expr_type(expr2_node, expr2_node->scope_sym_tab);
            *e_type = get_EopE_type(*e1_type, op, *e2_type);
            tmp_type = e_type;
        }
        else{
            tmp_type = e1_type;
        }
    }
    else{
        tmp_type = create_type();
        tmp_type->name = type_name;
    }
    // printf("e1_type->name = %s\n", terminal_string[e1_type->name]);
    switch(tmp_type->name){
        case BOOLEAN:
            tmp_type->width = WIDTH_BOOLEAN;
        break;
        case INTEGER:
            tmp_type->width = WIDTH_INTEGER;
        break;
        case REAL:
            tmp_type->width = WIDTH_REAL;
        break;        
        default:
            tmp_type->width = 0;
        break;
    }
    // printf("width allocated to tmp : %d\n", tmp_type->width);
    if(tmp_node->encl_fun_type_ptr == NULL){
        printf("encl_fun_type_ptr for tmp is NULL\n\n");
    }
    tmp_type->offset = tmp_node->encl_fun_type_ptr->typeinfo.module.curr_offset;
    tmp_node->encl_fun_type_ptr->typeinfo.module.curr_offset += tmp_type->width;
    hash_insert_ptr_val(tmp_node->scope_sym_tab->table, tmp_node->token.id.str, tmp_type);
    return tmp_node;
}

char *node2_tkn_str_val(tree_node *node)
{
    char *str_val = (char *)malloc( sizeof(char) * MAX_SYMBOL_LENGTH);
    switch(node->token.name){
        case ID:        
            strcpy(str_val, node->token.id.str);
        break;

        case NUM:
            sprintf(str_val, "#%d", node->token.num);
        break;

        case RNUM:
            sprintf(str_val, "#%lf", node->token.rnum);
        break;
        case TRUE:
            sprintf(str_val,"#1");
        break;
        case FALSE:
            sprintf(str_val,"#0");
        break;
        default:
            strcpy(str_val, "");
        break;
    }

    return str_val;
}

tac_op get_tac_op(tree_node *node){
    token_name t = node->token.name;
    tac_op op;
    switch(t){
        case PLUS:
            op = PLUS_OP;
        break;
        case MINUS:
            op = MINUS_OP;
        break;
        case MUL:
            op = MUL_OP;
        break;
        case DIV:
            op = DIV_OP;
        break;
        case GT:
            op = GT_OP;
        break;
        case GE:
            op = GE_OP;
        break;
        case LT:
            op = LT_OP;
        break;
        case LE:
            op = LE_OP;
        break;
        case NE:
            op = NE_OP;
        break;
        case EQ:
            op = EQ_OP;
        break;
        case AND:
            op = AND_OP;
        break;
        case OR:
            op = OR_OP;
        break;
        case GET_VALUE:
            op = INPUT_OP;
        break;
        case PRINT:
            op = OUTPUT_OP;
        break;            
    }
    return op;
}

void code_emit(tac_op op, char *arg1, char *arg2, char *result, st_wrapper *curr_scope_table_ptr, type *encl_fun_type_ptr)
{
    if(op == LABEL_OP && arg1 == NULL)
            return;
        
    if(arg1){
        // printf("inserting , arg1 : %s, ", arg1);
    }
    if(arg2){
        // printf("inserting , arg2 : %s, ", arg2);
    }
    if(result){
        // printf("inserting , result : %s", result);
    }
    // printf("\n");
    quadruples[quad_count].op = op;
    quadruples[quad_count].arg1 = arg1;
    quadruples[quad_count].arg2 = arg2;
    quadruples[quad_count].result = result;
    quadruples[quad_count].curr_scope_table_ptr = curr_scope_table_ptr;
    quadruples[quad_count].encl_fun_type_ptr = encl_fun_type_ptr;
    quad_count++;
}

void module_first_time(tree_node *module_node){
    // assign_next_label(module_node);
    char *fun_name;
    fun_name = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
    strcpy(fun_name, module_node->encl_fun_type_ptr->typeinfo.module.module_name);
    code_emit(FN_DEFN_OP, fun_name, NULL, NULL, module_node->scope_sym_tab,  module_node->encl_fun_type_ptr);                        
    char *fn_end_str = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
    strcpy(fn_end_str, module_node->encl_fun_type_ptr->typeinfo.module.module_name);
    strcat(fn_end_str, "_end");
    module_node->label.next_label = fn_end_str;
    // printf("Assigning module label : %s\n", fun_name);
}

void whileloop_first_time(tree_node *while_node){
 
    char *begin = newlabel();
    tree_node *expr_node = while_node->leftmost_child;
    tree_node *stmts_node = while_node->rightmost_child;
    expr_node->label.boolean.true_label = newlabel();
    assign_next_label(while_node);
    expr_node->label.boolean.false_label = while_node->label.next_label;
    stmts_node->label.next_label = begin;
    while_node->label.cnstrct_code_begin = begin;
    code_emit(LABEL_OP, begin, NULL, NULL, while_node->scope_sym_tab,  while_node->encl_fun_type_ptr);
    
}

void forloop_first_time(tree_node *forloop_node){

    assign_next_label(forloop_node);
    tree_node *iter_var_node = get_nth_child(forloop_node, 1);
    tree_node *range_low_node = get_nth_child(forloop_node, 2);
    tree_node *range_high_node = get_nth_child(forloop_node, 3);

    code_emit(ASSIGN_OP, node2_tkn_str_val(range_low_node), NULL, node2_tkn_str_val(iter_var_node), forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
    tree_node *tmp_node = newtemp(iter_var_node, NO_MATCHING_OP, NULL, TYPE_ERROR);
    forloop_node->label.cnstrct_code_begin = newlabel();
    code_emit(LABEL_OP, forloop_node->label.cnstrct_code_begin, NULL, NULL, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
    code_emit(LE_OP, node2_tkn_str_val(iter_var_node), node2_tkn_str_val(range_high_node), tmp_node->addr, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
    char *condn_true_lbl = newlabel();
    code_emit(IF_TRUE_GOTO_OP, node2_tkn_str_val(tmp_node), NULL, condn_true_lbl, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
    code_emit(GOTO_UNCOND_OP, forloop_node->label.next_label, NULL, NULL, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
    code_emit(LABEL_OP, condn_true_lbl, NULL, NULL, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
}

void conditional_node_first_time(tree_node *conditional_node){
    assign_next_label(conditional_node);
    if(switch_test_label)
        free(switch_test_label);
    switch_test_label = newlabel();
    code_emit(GOTO_UNCOND_OP, switch_test_label, NULL, NULL, conditional_node->scope_sym_tab,  conditional_node->encl_fun_type_ptr);
    switch_tbl_entry_num = 0;
}

void term_node_second_time(tree_node *term_node){
        // printf("Created new temp for term/relationalexpr\n");
    int num_child = term_node->num_child;
    if(num_child == 1){
        term_node->addr = term_node->leftmost_child->addr;
    }
    else if(num_child == 3){                            
        tree_node *first_child, *second_child, *third_child;
        first_child = get_nth_child(term_node, 1);
        second_child = get_nth_child(term_node, 2);
        third_child = get_nth_child(term_node, 3);
        tree_node *addr_node = newtemp(first_child, get_operator(second_child), third_child, TYPE_ERROR);
        term_node->addr = addr_node->addr;
        code_emit(get_tac_op(second_child), first_child->addr,third_child->addr, term_node->addr, term_node->scope_sym_tab,  term_node->encl_fun_type_ptr);
    }
    if(term_node->parent->sym.nt == WHILELOOP){
        // printf("leacving term and part of while\n");
        code_emit(IF_TRUE_GOTO_OP, term_node->addr, term_node->label.boolean.true_label, NULL, term_node->scope_sym_tab,  term_node->encl_fun_type_ptr);
        code_emit(GOTO_UNCOND_OP, term_node->label.boolean.false_label, NULL, NULL, term_node->scope_sym_tab,  term_node->encl_fun_type_ptr);
        code_emit(LABEL_OP, term_node->label.boolean.true_label, NULL, NULL, term_node->scope_sym_tab,  term_node->encl_fun_type_ptr);
    }
}

void rexpr_node_second_time(tree_node *rexpr_node){
    
    tree_node *first_child, *second_child, *third_child;
    first_child = get_nth_child(rexpr_node, 1);
    second_child = get_nth_child(rexpr_node, 2);
    third_child = get_nth_child(rexpr_node, 3);
    tree_node *addr_node = newtemp(first_child, get_operator(second_child), third_child, TYPE_ERROR);
    rexpr_node->addr = addr_node->addr;
    code_emit(get_tac_op(second_child), first_child->addr,third_child->addr, rexpr_node->addr, rexpr_node->scope_sym_tab,  rexpr_node->encl_fun_type_ptr);
    // code_emit(GOTO_UNCOND_OP, rexpr_node->label.boolean.false_label, NULL, NULL);
    if(rexpr_node->parent->sym.nt == WHILELOOP){
        code_emit(IF_TRUE_GOTO_OP, rexpr_node->addr, rexpr_node->label.boolean.true_label, NULL, rexpr_node->scope_sym_tab,  rexpr_node->encl_fun_type_ptr);
        code_emit(GOTO_UNCOND_OP, rexpr_node->label.boolean.false_label, NULL, NULL, rexpr_node->scope_sym_tab,  rexpr_node->encl_fun_type_ptr);
        code_emit(LABEL_OP, rexpr_node->label.boolean.true_label, NULL, NULL, rexpr_node->scope_sym_tab,  rexpr_node->encl_fun_type_ptr);
    }

}

void expr_n_nuaexpr_second_time(tree_node *ast_node){
    int num_child = ast_node->num_child;
    int num_loop_iter = (num_child + 1) / 2;
    char *temp, *prev_temp;
    tree_node *child = ast_node->leftmost_child;
    tree_node *operand1, *operand2, *operator_node;
    operand1 = child;
    operator_node = child->sibling;
    operand2 = operator_node->sibling;
    tree_node *tmp_node;

    for(int i = 0; i < num_loop_iter && operator_node != NULL; i++)
    {                            
        // printf("Created new temp for expression/nonunary\n");
        operator op = get_operator(operator_node);
        tmp_node = newtemp(operand1, op, operand2, TYPE_ERROR);
        temp = tmp_node->addr;                             
        
        if(i == 0){                                                               
            code_emit(get_tac_op(operator_node), operand1->addr, operand2->addr, temp, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);
        }
        else{
            code_emit(get_tac_op(operator_node), prev_temp, operand2->addr, temp, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);
        }
        operand1 = operand2;
        operator_node = operand2->sibling;
        if(operator_node)
            operand2 = operator_node->sibling;
        prev_temp = temp;
    }
    ast_node->addr = temp;
    if(ast_node->parent->sym.nt == WHILELOOP){
        // printf("leacving expr & nonunary and part of while\n");
        code_emit(IF_TRUE_GOTO_OP, ast_node->addr, ast_node->label.boolean.true_label, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);
        code_emit(GOTO_UNCOND_OP, ast_node->label.boolean.false_label, NULL, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);
        code_emit(LABEL_OP, ast_node->label.boolean.true_label, NULL, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);
    }
}

void uexpr_second_time(tree_node *uexpr_node){
    // printf("Created new temp for unaryarithmetic\n");
    tree_node *expr_node = uexpr_node->rightmost_child;
    tree_node *tmp_node = newtemp(expr_node, NO_MATCHING_OP, NULL, TYPE_ERROR);
    uexpr_node->addr = tmp_node->addr;
    tree_node *op_node = uexpr_node->leftmost_child;
    tac_op op = (op_node->token.name == PLUS) ? UPLUS_OP : UMINUS_OP;
    /** 
     * ToDo : it could be UPLUS also, correct it
     */
    code_emit(op, expr_node->addr,NULL, uexpr_node->addr, uexpr_node->scope_sym_tab,  uexpr_node->encl_fun_type_ptr);
}

void var_node_second_time(tree_node *var_node){
    tree_node *id_node = var_node->leftmost_child;
    if(id_node->sibling->sym.t != EPSILON){        
        // printf("Created new temp for var\n");
        type *arr_type = (type*)key_search_recursive(id_node->scope_sym_tab, id_node->token.id.str, id_node->encl_fun_type_ptr, NULL);

        char *width = (char*) malloc(sizeof(char) * MAX_LABEL_LEN);
        
        int width_val = 0;
        
        switch(arr_type->typeinfo.array.primitive_type){
            case BOOLEAN:
                snprintf(width, MAX_LABEL_LEN, "#%d", WIDTH_BOOLEAN);       
                width_val = WIDTH_BOOLEAN;
            break;

            case INTEGER:
                snprintf(width, MAX_LABEL_LEN, "#%d", WIDTH_INTEGER);       
                width_val = WIDTH_INTEGER;
            break;

            case REAL:
                snprintf(width, MAX_LABEL_LEN, "#%d", WIDTH_REAL);       
                width_val = WIDTH_REAL;
            break;        

            default:
                snprintf(width, MAX_LABEL_LEN, "#%d", 0);
            break;
        }                       
        tree_node *index_node = id_node->sibling;
        char *index = index_node->addr;
        
        if(index_node->token.name == ID){
            index_node = newtemp(index_node, NO_MATCHING_OP, NULL, TYPE_ERROR);
            code_emit(MUL_OP,id_node->sibling->addr, width, index, var_node->scope_sym_tab,  var_node->encl_fun_type_ptr);
        }
        else{
            snprintf(index, MAX_LABEL_LEN, "%d", index_node->token.num * width_val);
        }

        tree_node *tmp_node = newtemp(id_node, NO_MATCHING_OP, NULL, TYPE_ERROR);
        var_node->addr = tmp_node->addr;

        /**
         * @brief Dynamic array code
         */
        if(arr_type->typeinfo.array.is_dynamic.range_low || arr_type->typeinfo.array.is_dynamic.range_high || index_node->token.name == ID){
            /**
             * @brief Convert low and high ranges to strings                                 * 
             */

            char *low_range, *high_range;
            if(arr_type->typeinfo.array.is_dynamic.range_low){
                low_range = arr_type->typeinfo.array.range_low.lexeme;
            }
            else{
                low_range = (char*) malloc(sizeof(char) * MAX_LEXEME_LEN);
                snprintf(low_range, MAX_LEXEME_LEN, "%d", arr_type->typeinfo.array.range_low.value);
            }

            if(arr_type->typeinfo.array.is_dynamic.range_high){
                high_range = arr_type->typeinfo.array.range_high.lexeme;
            }
            else{
                high_range = (char*) malloc(sizeof(char) * MAX_LEXEME_LEN);
                snprintf(high_range, MAX_LEXEME_LEN, "%d", arr_type->typeinfo.array.range_high.value);
            }

            /**
             * @brief Emit code for index bounds checking
             */
            tree_node *dummy_bool_node = create_tree_node();
            dummy_bool_node->encl_fun_type_ptr = index_node->encl_fun_type_ptr;
            dummy_bool_node->scope_sym_tab = index_node->scope_sym_tab;
            tree_node *condn_eval_temp = newtemp(dummy_bool_node, NO_MATCHING_OP, NULL, BOOLEAN);

            code_emit(LT_OP, node2_tkn_str_val(id_node->sibling), low_range, condn_eval_temp->addr, var_node->scope_sym_tab,  var_node->encl_fun_type_ptr);
            code_emit(EXIT_PROGRAM_IF_TRUE_OP, condn_eval_temp->addr, NULL, NULL, var_node->scope_sym_tab,  var_node->encl_fun_type_ptr);

            code_emit(GT_OP, node2_tkn_str_val(id_node->sibling), high_range, condn_eval_temp->addr, var_node->scope_sym_tab,  var_node->encl_fun_type_ptr);
            code_emit(EXIT_PROGRAM_IF_TRUE_OP, condn_eval_temp->addr, NULL, NULL, var_node->scope_sym_tab,  var_node->encl_fun_type_ptr);
        }

        code_emit(ARRAY_ACCESS_OP, id_node->addr, index, var_node->addr, var_node->scope_sym_tab,  var_node->encl_fun_type_ptr);
    }
    else{
        var_node->addr = id_node->addr;
    }
}

void assign_node_second_time(tree_node *assign_node){
    tree_node *rvalue_node = assign_node->rightmost_child;
    tree_node *lvalue_id_node = assign_node->leftmost_child;
    tree_node *index_node = lvalue_id_node->sibling;    
    
    char *lvalue;
    if(index_node->sym.t == EPSILON)
        code_emit(ASSIGN_OP, rvalue_node->addr, NULL, lvalue_id_node->addr, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);
    else{
        /**
         * @brief Array type
         * Check if dynamic and generate code accordingly
         */
        type *arr_type = (type*)key_search_recursive(lvalue_id_node->scope_sym_tab, lvalue_id_node->token.id.str,
                            lvalue_id_node->encl_fun_type_ptr, NULL);
        /**
         * @brief Dynamic array code
         */
        if(arr_type->typeinfo.array.is_dynamic.range_low || arr_type->typeinfo.array.is_dynamic.range_high 
            || index_node->token.name == ID){
            /**
             * @brief Convert low and high ranges to strings                                 * 
             */

            char *low_range, *high_range;
            if(arr_type->typeinfo.array.is_dynamic.range_low){
                low_range = arr_type->typeinfo.array.range_low.lexeme;
            }
            else{
                low_range = (char*) malloc(sizeof(char) * MAX_LEXEME_LEN);
                snprintf(low_range, MAX_LEXEME_LEN, "%d", arr_type->typeinfo.array.range_low.value);
            }

            if(arr_type->typeinfo.array.is_dynamic.range_high){
                high_range = arr_type->typeinfo.array.range_high.lexeme;
            }
            else{
                high_range = (char*) malloc(sizeof(char) * MAX_LEXEME_LEN);
                snprintf(high_range, MAX_LEXEME_LEN, "%d", arr_type->typeinfo.array.range_high.value);
            }                                
            
            tree_node *dummy_bool_node = create_tree_node();
            dummy_bool_node->encl_fun_type_ptr = index_node->encl_fun_type_ptr;
            dummy_bool_node->scope_sym_tab = index_node->scope_sym_tab;
            tree_node *condn_eval_temp = newtemp(dummy_bool_node, NO_MATCHING_OP, NULL, BOOLEAN);

            code_emit(LT_OP, node2_tkn_str_val(index_node), low_range, condn_eval_temp->addr, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);
            code_emit(EXIT_PROGRAM_IF_TRUE_OP, condn_eval_temp->addr, NULL, NULL, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);

            code_emit(GT_OP, node2_tkn_str_val(index_node), high_range, condn_eval_temp->addr, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);
            code_emit(EXIT_PROGRAM_IF_TRUE_OP, condn_eval_temp->addr, NULL, NULL, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);
        }

        char *width = (char*) malloc(sizeof(char) * MAX_LABEL_LEN);
        int width_val = 0;
        switch(arr_type->typeinfo.array.primitive_type){
            case BOOLEAN:
                snprintf(width, MAX_LABEL_LEN, "#%d", WIDTH_BOOLEAN);       
                width_val = WIDTH_BOOLEAN;
            break;

            case INTEGER:
                snprintf(width, MAX_LABEL_LEN, "#%d", WIDTH_INTEGER);       
                width_val = WIDTH_INTEGER;
            break;

            case REAL:
                snprintf(width, MAX_LABEL_LEN, "#%d", WIDTH_REAL);       
                width_val = WIDTH_REAL;
            break;        

            default:
                snprintf(width, MAX_LABEL_LEN, "#%d", 0);
            break;
        }                            
        
        char *index;
        
        if(index_node->token.name == ID){
            tree_node *arr_locn_node = newtemp(index_node, NO_MATCHING_OP, NULL, TYPE_ERROR);
            index = arr_locn_node->addr;
            code_emit(MUL_OP,node2_tkn_str_val(index_node), width, index, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);
        }
        else{
            index = (char*) malloc(sizeof(char) * MAX_LEXEME_LEN);
            snprintf(index, MAX_LEXEME_LEN, "%d", width_val * index_node->token.num);
        }

        code_emit(INDEXED_COPY_OP,index, rvalue_node->addr, lvalue_id_node->addr, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);
    }

    
    // assign_node->label.next_label = newlabel();                            

    if(assign_node->parent->label.next_label == NULL ){


        code_emit(LABEL_OP, assign_node->label.next_label, NULL, NULL, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);
    }
    else if(strcmp(assign_node->label.next_label, assign_node->parent->label.next_label) != 0){


        code_emit(LABEL_OP, assign_node->label.next_label, NULL, NULL, assign_node->scope_sym_tab,  assign_node->encl_fun_type_ptr);
    }
}

void iostmt_node_second_time(tree_node *iostmt_node){
    tree_node *action_node = iostmt_node->leftmost_child;                        
    if(action_node->token.name == GET_VALUE){
        tree_node *id_node = iostmt_node->rightmost_child;
        code_emit(INPUT_OP, id_node->token.id.str, NULL, NULL, iostmt_node->scope_sym_tab,  iostmt_node->encl_fun_type_ptr);
    }
    else{   //print
        tree_node *var_node = iostmt_node->rightmost_child;
        code_emit(OUTPUT_OP, var_node->addr, NULL, NULL, iostmt_node->scope_sym_tab,  iostmt_node->encl_fun_type_ptr);
    }
    
    // iostmt_node->label.next_label = newlabel();                            

    if(iostmt_node->parent->label.next_label == NULL ){


        code_emit(LABEL_OP, iostmt_node->label.next_label, NULL, NULL, iostmt_node->scope_sym_tab,  iostmt_node->encl_fun_type_ptr);
    }
    else if(strcmp(iostmt_node->label.next_label, iostmt_node->parent->label.next_label) != 0){


        code_emit(LABEL_OP, iostmt_node->label.next_label, NULL, NULL, iostmt_node->scope_sym_tab,  iostmt_node->encl_fun_type_ptr);
    }
}

void fncall_node_second_time(tree_node *fncall_node){
    tree_node *inp_params_node = fncall_node->rightmost_child;
    tree_node *outp_params_node = fncall_node->leftmost_child;
    int inp_param_num = inp_params_node->num_child;
    int outp_param_num = outp_params_node->num_child;
    
    tree_node *outp_param = outp_params_node->leftmost_child;
    
    stack *stack_ptr = stack_init();
    
    for(int i = 0; i < outp_param_num; i++){
        push(stack_ptr, outp_param);    // reverse the order of params
        outp_param = outp_param->sibling;
    }

    for(int i = 0; i < outp_param_num; i++){
        outp_param = pop(stack_ptr);
        code_emit(OUTP_PARAM_OP, outp_param->addr, NULL, NULL, fncall_node->scope_sym_tab,  fncall_node->encl_fun_type_ptr);
    }

    tree_node *inp_param = inp_params_node->leftmost_child;
    for(int i = 0; i < inp_param_num; i++){                            
        push(stack_ptr, inp_param);
        inp_param = inp_param->sibling;
    }

    for(int i = 0; i< inp_param_num; i++){
        inp_param = pop(stack_ptr);
        code_emit(INP_PARAM_OP, inp_param->addr, NULL, NULL, fncall_node->scope_sym_tab,  fncall_node->encl_fun_type_ptr);
    }

    free(stack_ptr->arr);   // free up space used by stack
    free(stack_ptr);

    char *param_num_str = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
    snprintf(param_num_str, MAX_LABEL_LEN, "%d", inp_param_num);
    code_emit(PROC_CALL_OP, get_nth_child(fncall_node, 2)->addr, param_num_str, NULL, fncall_node->scope_sym_tab,  fncall_node->encl_fun_type_ptr);
    
    // fncall_node->label.next_label = newlabel();                            

    if(fncall_node->parent->label.next_label == NULL ){


        code_emit(LABEL_OP, fncall_node->label.next_label, NULL, NULL, fncall_node->scope_sym_tab,  fncall_node->encl_fun_type_ptr);
    }
    else if(strcmp(fncall_node->label.next_label, fncall_node->parent->label.next_label) != 0){


        code_emit(LABEL_OP, fncall_node->label.next_label, NULL, NULL, fncall_node->scope_sym_tab,  fncall_node->encl_fun_type_ptr);
    }
}

void whileloop_node_second_time(tree_node *whileloop_node){
    code_emit(GOTO_UNCOND_OP, whileloop_node->label.cnstrct_code_begin, NULL, NULL, whileloop_node->scope_sym_tab,  whileloop_node->encl_fun_type_ptr);
    if(whileloop_node->parent->label.next_label == NULL ){


        code_emit(LABEL_OP, whileloop_node->label.next_label, NULL, NULL, whileloop_node->scope_sym_tab,  whileloop_node->encl_fun_type_ptr);
    }
    else if(strcmp(whileloop_node->label.next_label, whileloop_node->parent->label.next_label) != 0){


        code_emit(LABEL_OP, whileloop_node->label.next_label, NULL, NULL, whileloop_node->scope_sym_tab,  whileloop_node->encl_fun_type_ptr);
    }
    // printf("And whileloop second time visiting emits after jmp begin, label %s\n", whileloop_node->label.next_label);
    // whileloop_node->label.next_label = newlabel();                            
}

void forloop_node_second_time(tree_node *forloop_node){
    /**
     * @brief Increment iterating variable
     */
    
    tree_node *iter_node = get_nth_child(forloop_node, 1);
    code_emit(PLUS_OP, iter_node->addr, "#1", iter_node->addr, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
    code_emit(GOTO_UNCOND_OP, forloop_node->label.cnstrct_code_begin, NULL, NULL, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);

    if(forloop_node->parent->label.next_label == NULL ){


        code_emit(LABEL_OP, forloop_node->label.next_label, NULL, NULL, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
    }
    else if(strcmp(forloop_node->label.next_label, forloop_node->parent->label.next_label) != 0){


        code_emit(LABEL_OP, forloop_node->label.next_label, NULL, NULL, forloop_node->scope_sym_tab,  forloop_node->encl_fun_type_ptr);
    }
}

void conditional_node_second_time(tree_node *conditional_node){
    tree_node *switch_var_node = conditional_node->leftmost_child;
    tree_node *dummy_bool_node = create_tree_node();
    dummy_bool_node->encl_fun_type_ptr = switch_var_node->encl_fun_type_ptr;
    dummy_bool_node->scope_sym_tab = switch_var_node->scope_sym_tab;
    tree_node *tmp_var = newtemp(dummy_bool_node, NO_MATCHING_OP, NULL, BOOLEAN);
    int num_cases = get_nth_child(conditional_node, 2)->num_child;

    /**
     * @brief Emit test label
     */

    code_emit(LABEL_OP, switch_test_label, NULL, NULL, conditional_node->scope_sym_tab,  conditional_node->encl_fun_type_ptr);

    for(int i=0; i<num_cases; i++){
        code_emit(EQ_OP, switch_table[i].value, switch_var_node->addr, tmp_var->addr, conditional_node->scope_sym_tab,  conditional_node->encl_fun_type_ptr);
        code_emit(IF_TRUE_GOTO_OP, tmp_var->addr, switch_table[i].label, NULL, conditional_node->scope_sym_tab,  conditional_node->encl_fun_type_ptr);
    }

    tree_node *default_node = conditional_node->rightmost_child;
    if(default_node->sym.is_terminal == false){
        code_emit(GOTO_UNCOND_OP, switch_table[switch_tbl_entry_num-1].label, NULL, NULL, conditional_node->scope_sym_tab,  conditional_node->encl_fun_type_ptr);
    }

    if(conditional_node->parent->label.next_label == NULL ){


        code_emit(LABEL_OP, conditional_node->label.next_label, NULL, NULL, conditional_node->scope_sym_tab,  conditional_node->encl_fun_type_ptr);
    }
    else if(strcmp(conditional_node->label.next_label, conditional_node->parent->label.next_label) != 0){


        code_emit(LABEL_OP, conditional_node->label.next_label, NULL, NULL, conditional_node->scope_sym_tab,  conditional_node->encl_fun_type_ptr);
    }
}

void declare_node_second_time(tree_node *declare_node){
    tree_node *id_node = declare_node->leftmost_child->leftmost_child;
    bool is_dynamic = false;
    type *id_type_ptr;
    while(id_node != NULL){
        id_type_ptr = (type *)key_search_recursive(id_node->scope_sym_tab, id_node->token.id.str, id_node->encl_fun_type_ptr, NULL);
        if(id_type_ptr && id_type_ptr->name == ARRAY){
            if(id_type_ptr->typeinfo.array.is_dynamic.range_low)
                is_dynamic = true;
            if(id_type_ptr->typeinfo.array.is_dynamic.range_high)
                is_dynamic = true;
            if(is_dynamic)
                code_emit(DECLARE_DYN_ARR_OP, id_node->token.id.str, NULL, NULL, id_node->scope_sym_tab, id_node->encl_fun_type_ptr);
            else
                code_emit(DECLARE_STATIC_ARR_OP, id_node->token.id.str, NULL, NULL, id_node->scope_sym_tab, id_node->encl_fun_type_ptr);
        }
        id_node = id_node->sibling;
    }
}


void generate_ir(tree_node *ast_node)
{
    while(ast_node != NULL){

        if(ast_node->visited == false){
            // printf("Visiting first time: ");

            ast_node->visited = true;
            
            if(ast_node->sym.is_terminal == false){
                switch(ast_node->sym.nt){
                    case NTMODULE:
                    case DRIVERMODULE:
                    {                        
                        module_first_time(ast_node);
                    }
                    break;     
                    case WHILELOOP:
                    {
                        whileloop_first_time(ast_node);
                    }   
                    break;         
                    case ASSIGNMENTSTMT:
                    case IOSTMT:
                    case FUNCTIONCALLSTMT:
                    {
                        assign_next_label(ast_node);
                    }
                    break;
                    case FORLOOP:
                    {
                        forloop_first_time(ast_node);                        
                    }
                    break;
                    case CONDITIONALSTMT:
                    {
                        conditional_node_first_time(ast_node);
                    }
                    break;
                    case CASESTMT:
                    {
                        ast_node->label.cnstrct_code_begin = newlabel();
                        code_emit(LABEL_OP, ast_node->label.cnstrct_code_begin, NULL, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);
                    }
                    break;
                    case DEFAULTSTMT:
                    {
                        ast_node->label.cnstrct_code_begin = newlabel();
                        code_emit(LABEL_OP, ast_node->label.cnstrct_code_begin, NULL, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);
                    }
                    break;
                    case STATEMENTS:
                    {
                        assign_next_label(ast_node);
                    }
                }
            }

            if(ast_node->leftmost_child != NULL){
                ast_node = ast_node->leftmost_child;
            }
        }
        else{
            ast_node->visited = false;
            // printf("Visiting second time: ");

            if(ast_node->sym.is_terminal == false){
                switch(ast_node->sym.nt){
                    case TERM:                    
                    {                        
                        term_node_second_time(ast_node);
                    }
                    break;
                    case RELATIONALEXPR:
                    {                        
                        rexpr_node_second_time(ast_node);
                    }
                    break;
                    case EXPRESSION:
                    case NONUNARYARITHMETICEXPR:
                    {
                        expr_n_nuaexpr_second_time(ast_node);
                    }
                    break;
                    case UNARYARITHMETICEXPR:
                    {                           
                        uexpr_second_time(ast_node);
                    }
                    break;
                    case VAR:
                    {                        
                        var_node_second_time(ast_node);
                    }
                    break;
                    case ASSIGNMENTSTMT:
                    {
                        assign_node_second_time(ast_node);                        
                    }
                    break;
                    case IOSTMT:
                    {
                        iostmt_node_second_time(ast_node);
                    }
                    break;
                    case FUNCTIONCALLSTMT:
                    {
                        fncall_node_second_time(ast_node);                        
                    }
                    break;
                    case WHILELOOP:
                    {
                        whileloop_node_second_time(ast_node);

                    }
                    break;
                    case FORLOOP:
                    {
                        forloop_node_second_time(ast_node);
                    }
                    break;
                    case CONDITIONALSTMT:
                    {
                        conditional_node_second_time(ast_node);
                    }
                    break;
                    case NTMODULE:
                    case DRIVERMODULE:
                    {   
                        code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);                        
                        code_emit(RETURN_OP, NULL, NULL, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);                        
                    }
                    break;
                    case CASESTMT:
                    {
                        tree_node *switch_node = ast_node->parent->parent;
                        code_emit(GOTO_UNCOND_OP, switch_node->label.next_label, NULL, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);
                        
                        switch_table[switch_tbl_entry_num].label = ast_node->label.cnstrct_code_begin;
                        switch_table[switch_tbl_entry_num].value = ast_node->leftmost_child->addr;
                        switch_tbl_entry_num++;
                    }
                    break;
                    case DEFAULTSTMT:
                    {
                        tree_node *switch_node = ast_node->parent->parent;
                        code_emit(GOTO_UNCOND_OP, switch_node->label.next_label, NULL, NULL, ast_node->scope_sym_tab,  ast_node->encl_fun_type_ptr);                        
                        switch_table[switch_tbl_entry_num].label = ast_node->label.cnstrct_code_begin;
                        switch_table[switch_tbl_entry_num].value = "default_case";
                        switch_tbl_entry_num++;
                    }
                    break;
                    case DECLARESTMT:
                    {
                        declare_node_second_time(ast_node);
                    }
                    break;
                }
            }
            else{
                switch(ast_node->sym.t){
                    case ID:                    
                    case RNUM:                        
                    case NUM:
                    case TRUE:
                    case FALSE:    
                    {
                        ast_node->addr = node2_tkn_str_val(ast_node);
                    }                    
                    break;
                }
            }

            if(ast_node->sibling){
                ast_node = ast_node->sibling;
            }
            else{
                ast_node = ast_node->parent;
            }
        }

    }
}

