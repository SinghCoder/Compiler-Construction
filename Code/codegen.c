#include "codegen.h"
#include <string.h>
#include <stdlib.h>

void print_quadruples()
{
    printf("%-15s | ","operator");
    printf("%-15s | ","arg1");
    printf("%-15s | ","arg2");
    printf("%-15s | ","result");
    printf("\n");
    printf("%-15s | ","---------------");
    printf("%-15s | ","---------------");
    printf("%-15s | ","---------------");
    printf("%-15s | ","---------------");
    printf("\n\n");

    for(int i = 0; i < quad_count; i++){
        printf("%-15s | ", tac_op_str[quadruples[i].op] );
        if(quadruples[i].arg1){
            printf("%-15s | ", quadruples[i].arg1);
        }
        else
            printf("%-15s | ","(empty)");
        if(quadruples[i].arg2){
            printf("%-15s | ", quadruples[i].arg2);
        }
        else
            printf("%-15s | ","(empty)");
        if(quadruples[i].result){
            printf("%-15s | ", quadruples[i].result);
        }
        else
            printf("%-15s | ","(empty)");
        printf("\n");
    }
}

void codegen_init()
{
    label_count = 0;
    temp_count = 0;
    quad_count = 0;
    char *tac_op_str_tmp[NUM_TAC_OP] = {"+", "-", "*", "/", ">=", ">", "<=", "<", "==", "!=", "&&",
                                         "||", "uminus", "label", "input", "output", "assign", "jmp",
                                         "jmp_if_true", "jmp_if_false", "param" ,"call", "indexed_copy",
                                         "array_access", "switch_op", "goto_case", "default"};
    for(int i=0; i<NUM_TAC_OP; i++){        
        strcpy(tac_op_str[i], tac_op_str_tmp[i]);
        // printf("tac_op_str[%d] = %s\n", i, tac_op_str[i]);
    }
}

char *newlabel()
{
    char *label = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
    sprintf(label, "label_%d :", label_count);
    label_count++;
    return label;
}

tree_node *newtemp(tree_node *expr1_node, operator op, tree_node *expr2_node)
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
    e1_type = create_type();
    e2_type = create_type();
    e_type = create_type();
    *e1_type = get_expr_type(expr1_node, expr1_node->scope_sym_tab);
    if(expr2_node != NULL){
        *e2_type = get_expr_type(expr2_node, expr2_node->scope_sym_tab);
        *e_type = get_EopE_type(*e1_type, op, *e2_type);
        tmp_type = e_type;
    }
    else{
        tmp_type = e1_type;
    }
    // printf("e1_type->name = %s\n", terminal_string[e1_type->name]);
    switch(e1_type->name){
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

char *to_string(tree_node *node)
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
        case FALSE:
            sprintf(str_val,"#%s", node->token.id.str);
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

void code_emit(tac_op op, char *arg1, char *arg2, char *result, tree_node *node_arg1, tree_node *node_arg2, tree_node *node_result)
{
    if(arg1){
        // printf("inserting , arg1 : %s, ", arg1);
    }
    if(arg2){
        // printf("inserting , arg2 : %s, ", arg2);
    }
    if(result){
        // printf("inserting , result : %s\n", result);
    }

    quadruples[quad_count].op = op;
    quadruples[quad_count].arg1 = arg1;
    quadruples[quad_count].node_arg1 = node_arg1;
    quadruples[quad_count].arg2 = arg2;
    quadruples[quad_count].node_arg2 = node_arg2;
    quadruples[quad_count].result = result;
    quadruples[quad_count].node_res = node_result;
    quad_count++;
}

void generate_ir(tree_node *ast_node)
{
    while(ast_node != NULL){

        if(ast_node->visited == false){
            // printf("Visiting first time: ");
            // print_symbol_(ast_node);
            ast_node->visited = true;
            
            if(ast_node->sym.is_terminal == false){
                switch(ast_node->sym.nt){
                    case NTMODULE:
                    case DRIVERMODULE:
                    {                        
                        char *fun_name;
                        fun_name = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
                        strcpy(fun_name, ast_node->encl_fun_type_ptr->typeinfo.module.module_name);
                        strcat(fun_name, " :");
                        code_emit(LABEL_OP, fun_name, NULL, NULL, NULL, NULL, NULL);
                    }
                    break;                    
                }
            }

            if(ast_node->leftmost_child != NULL){
                ast_node = ast_node->leftmost_child;
            }
        }
        else{
            ast_node->visited = false;
            // printf("Visiting second time: ");
            // print_symbol_(ast_node);
            if(ast_node->sym.is_terminal == false){
                switch(ast_node->sym.nt){
                    case TERM:
                    case RELATIONALEXPR:
                    {                        
                        // printf("Created new temp for term/relationalexpr\n");
                        int num_child = ast_node->num_child;
                        if(num_child == 1){
                            ast_node->addr = ast_node->leftmost_child->addr;
                        }
                        else if(num_child == 3){                            
                            tree_node *first_child, *second_child, *third_child;
                            first_child = get_nth_child(ast_node, 1);
                            second_child = get_nth_child(ast_node, 2);
                            third_child = get_nth_child(ast_node, 3);
                            tree_node *addr_node = newtemp(first_child, get_operator(second_child), third_child);
                            ast_node->addr = addr_node->addr;
                            code_emit(get_tac_op(second_child), first_child->addr,third_child->addr, ast_node->addr, NULL, NULL, NULL);
                        }
                    }
                    break;
                    case EXPRESSION:
                    case NONUNARYARITHMETICEXPR:
                    {
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
                            tmp_node = newtemp(operand1, op, operand2);
                            temp = tmp_node->addr;                             
                            
                            if(i == 0){                                                               
                                code_emit(get_tac_op(operator_node), operand1->addr, operand2->addr, temp, NULL, NULL, NULL);
                            }
                            else{
                                code_emit(get_tac_op(operator_node), prev_temp, operand2->addr, temp, NULL, NULL, NULL);
                            }
                            operand1 = operand2;
                            operator_node = operand2->sibling;
                            if(operator_node)
                                operand2 = operator_node->sibling;
                            prev_temp = temp;
                        }
                        ast_node->addr = temp;
                    }
                    break;
                    case UNARYARITHMETICEXPR:
                    {                           
                        // printf("Created new temp for unaryarithmetic\n");
                        tree_node *expr_node = ast_node->rightmost_child;
                        tree_node *tmp_node = newtemp(expr_node, NO_MATCHING_OP, NULL);
                        ast_node->addr = tmp_node->addr;
                        /** 
                         * ToDo : it could be UPLUS also, correct it
                         */
                        code_emit(UMINUS_OP, expr_node->addr,NULL, ast_node->addr, NULL, NULL, NULL);
                    }
                    break;
                    case VAR:
                    {                        
                        tree_node *id_node = ast_node->leftmost_child;
                        if(id_node->sibling->sym.t != EPSILON){
                            tree_node *index_node = newtemp(id_node->sibling, NO_MATCHING_OP, NULL);
                            char *index = index_node->addr;
                            // printf("Created new temp for var\n");
                            type *arr_type = (type*)key_search_recursive(id_node->scope_sym_tab, id_node->token.id.str, id_node->encl_fun_type_ptr, NULL);
                            char *width = (char*) malloc(sizeof(char) * MAX_LABEL_LEN);
                            snprintf(width, MAX_LABEL_LEN, "#%d", arr_type->width);
                            code_emit(MUL_OP,id_node->sibling->addr, width, index, NULL, NULL, NULL);

                            tree_node *tmp_node = newtemp(id_node, NO_MATCHING_OP, NULL);
                            ast_node->addr = tmp_node->addr;

                            code_emit(ARRAY_ACCESS_OP, id_node->addr, index, ast_node->addr, NULL, NULL, NULL);
                        }
                        else{
                            ast_node->addr = id_node->addr;
                        }
                    }
                    break;
                    case ASSIGNMENTSTMT:
                    {
                        tree_node *rvalue_node = ast_node->rightmost_child;
                        tree_node *lvalue_id_node = ast_node->leftmost_child;
                        tree_node *index_node = lvalue_id_node->sibling;
                        char *lvalue;
                        code_emit(ASSIGN_OP, rvalue_node->addr, NULL, lvalue_id_node->addr, NULL ,NULL , lvalue_id_node);
                    }
                    break;
                    case IOSTMT:
                    {
                        tree_node *action_node = ast_node->leftmost_child;                        
                        if(action_node->token.name == GET_VALUE){
                            tree_node *id_node = ast_node->rightmost_child;
                            code_emit(INPUT_OP, id_node->token.id.str, NULL, NULL, NULL, NULL, NULL);
                        }
                        else{   //print
                            tree_node *var_node = ast_node->rightmost_child;
                            code_emit(OUTPUT_OP, var_node->addr, NULL, NULL, NULL, NULL, NULL);
                        }
                    }
                    break;
                    case FUNCTIONCALLSTMT:
                    {
                        tree_node *inp_params_node = ast_node->rightmost_child;
                        int param_num = inp_params_node->num_child;
                        tree_node *param = inp_params_node->leftmost_child;
                        for(int i = 0; i < param_num; i++){
                            code_emit(PARAM_OP, param->addr, NULL, NULL, NULL, NULL, NULL);
                            param = param->sibling;
                        }
                        char *param_num_str = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
                        snprintf(param_num_str, MAX_LABEL_LEN, "%d", param_num);
                        code_emit(PROC_CALL_OP, get_nth_child(ast_node, 2)->addr, param_num_str, NULL, NULL, NULL, NULL);
                    }
                    break;
                    case CONDITIONALSTMT:
                    {
                        code_emit(SWITCH_OP, ast_node->leftmost_child->addr, NULL, NULL, ast_node->leftmost_child, NULL, NULL);
                        tree_node *casestmts = ast_node->leftmost_child->sibling;

                        if(ast_node->rightmost_child->sym.t == EPSILON)    // if-else statement with choice as boolean
                        {
                            code_emit(IF_TRUE_GOTO_OP, casestmts->leftmost_child->leftmost_child->addr, NULL, NULL, casestmts->leftmost_child->leftmost_child, NULL, casestmts->leftmost_child->rightmost_child);
                            code_emit(IF_FALSE_GOTO_OP, casestmts->rightmost_child->leftmost_child->addr, NULL, NULL, casestmts->rightmost_child->leftmost_child, NULL, casestmts->rightmost_child->rightmost_child);
                        }
                        else    // normal switch statement with choice as integer
                        {
                            tree_node *casestmt = casestmts->leftmost_child;
                            
                            while(casestmt != NULL)
                            {
                                // char *case_match = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
                                // snprintf(case_match, MAX_LABEL_LEN, "%d", casestmt->leftmost_child->token.num);
                                code_emit(GOTO_IF_MATCHES, casestmt->leftmost_child->addr, NULL, NULL, casestmt->leftmost_child, NULL, casestmt->rightmost_child);
                                casestmt = casestmt->sibling;
                            }

                            //default_stmt
                            char *default_str = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
                            snprintf(default_str, MAX_LABEL_LEN, "%s", "default");
                            code_emit(DEFAULT_OP, default_str, NULL, NULL, ast_node->rightmost_child->leftmost_child, NULL, ast_node->rightmost_child->rightmost_child);
                        }
                    }
                    break;
                }
            }
            else{
                switch(ast_node->sym.t){
                    case ID:
                    case NUM:
                    case RNUM:    
                    case TRUE:
                    case FALSE:    
                    {
                        ast_node->addr = to_string(ast_node);
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