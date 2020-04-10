#include "semantic_analyzer.h"
#include "semantic_analyzerDef.h"
#include "parser.h"
#include "treeADT.h"
#include <stdlib.h>
#include <string.h>

type *create_type()
{
    type *t = (type*)malloc(sizeof(type));
    return t;
}

void free_recursive_sym_tab(st_wrapper *sym_tab)
{
    if(sym_tab == NULL)
        return;
    
    free_recursive_sym_tab(sym_tab->leftmost_child_table);
    st_wrapper *tmp = sym_tab->sibling_table;
    free(sym_tab);
    free_recursive_sym_tab(tmp);
}

void semantic_analyzer_init(){
    if(root_sym_tab_ptr)
    {
        free_recursive_sym_tab(root_sym_tab_ptr);
    }
    root_sym_tab_ptr = NULL;
    curr_sym_tab_ptr = NULL;
}

st_wrapper *symbol_table_init(){
    st_wrapper *sym_tab = (st_wrapper*) malloc( sizeof( st_wrapper ));
    sym_tab->parent_table = curr_sym_tab_ptr;
    if(sym_tab->parent_table)
        sym_tab->level_num = sym_tab->parent_table->level_num + 1;
    else
        sym_tab->level_num = 0;
    sym_tab->leftmost_child_table = NULL;
    sym_tab->rightmost_child_table = NULL;
    sym_tab->sibling_table = NULL;
    init_hash_table(sym_tab->table);

    if(root_sym_tab_ptr == NULL)
        root_sym_tab_ptr = sym_tab;
        
    return sym_tab;
}

void* key_search_recursive(st_wrapper *sym_table,char *lexeme, type *encl_fun_type_ptr, bool *is_outp_param){
    if(sym_table == NULL)
        return NULL;
    
    bool b = key_present_in_table(sym_table->table, lexeme);
	type *type_ptr;

    if(b == false){		
		type_ptr = (type *)key_search_recursive(sym_table->parent_table, lexeme, encl_fun_type_ptr, is_outp_param);
		//If not exists in any scope, check for function parameters
		if(type_ptr == NULL)
			type_ptr = check_encl_fun_params(encl_fun_type_ptr, lexeme, is_outp_param);
	}
        
    else 
        type_ptr = search_hash_table_ptr_val(sym_table->table, lexeme);
	
	return type_ptr;
}

bool is_new_scope_openable(nonterminal nt){
    switch(nt){
        case MAINPROGRAM:
        case DRIVERMODULE:
        case NTMODULE:
        case CONDITIONALSTMT:
        // case DEFAULTSTMT:
        case FORLOOP:
        case WHILELOOP:
            return true;
            break;
        
        default:
            return false;
            break;
    }
}
operator get_operator(tree_node *node){
    operator op;

    switch(node->sym.t){
        case PLUS:
        case MINUS:
        case MUL:
        case DIV:
            op = ARITH_OP;
            break;
        
        case LT:
        case LE:
        case GT:
        case GE:
        case NE:
        case EQ:
            op = REL_OP;
            break;
        
        case AND:
        case OR:
            op = LOGICAL_OP;
            break;
        
        default:
            op = NO_MATCHING_OP;
            break;
    }

    return op;
}

type get_EopE_type(type t1, operator op, type t2){
     printf("t1 : %s, t2 : %s\n", terminal_string[t1.name], terminal_string[t2.name]); 
    type e_type;

    switch(op){
        case ARITH_OP:
            if(t1.name == t2.name){
                if(t1.name == INTEGER){
                    e_type.name = INTEGER;
                }
                else if(t1.name == REAL){
                    e_type.name = REAL;
                }
                else{
                     printf("ARITHOP, both same but not int/real\n"); 
                    e_type.name = TYPE_ERROR;
                }
            }
            else{
                 printf("ARITHOP, both not same\n"); 
                e_type.name = TYPE_ERROR;
            }
        break;

        case REL_OP:

            if(t1.name == t2.name){
                if(t1.name == INTEGER){
                    e_type.name = BOOLEAN;
                }
                else if(t1.name == REAL){
                    e_type.name = BOOLEAN;
                }
                else{
                     printf("RELOP, both same but not int/real\n"); 
                    e_type.name = TYPE_ERROR;
                }
            }
            else{
                 printf("First : %s, second %s", terminal_string[t1.name], terminal_string[t2.name]); 
                 printf("RELOP, both not same\n"); 
                e_type.name = TYPE_ERROR;
            }

        break;

        case LOGICAL_OP:

            if(t1.name == t2.name){
                if(t1.name == BOOLEAN){
                    e_type.name = BOOLEAN;
                }
                else{
                    e_type.name = TYPE_ERROR;
                }
            }
            else{
                e_type.name = TYPE_ERROR;
            }

        break;

        default:
            e_type.name = TYPE_ERROR;
        break;
    }

    return e_type;
}

type get_expr_type(tree_node *expr_node, st_wrapper *sym_tab_ptr){
    int num_child = expr_node->num_child;
    type t;
    // printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^inside get expr type^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
    //  printf("    left : "); 
    // print_symbol_(expr_node->leftmost_child);
    //  printf("    top : "); 
    // print_symbol_(expr_node);    
    //  printf("    right : "); 
    // print_symbol_(expr_node->rightmost_child);
    //  printf("\n"); 
    // printf("num_child : %d\n", num_child);
    switch(num_child){
        case 0:
        {
            // printf("the term is %s\n", terminal_string[expr_node->sym.t]);
            switch(expr_node->sym.t){
                case NUM:
                    t.name = INTEGER;
                break;
                
                case RNUM:
                    t.name = REAL;
                break;
                
                case TRUE:
                case FALSE:
                    t.name = BOOLEAN;
                break;

                case ID:
                {
                    t = *(type*)key_search_recursive(expr_node->scope_sym_tab, expr_node->token.id.str, expr_node->encl_fun_type_ptr, NULL);
                    if(t.name == ARRAY)
                        t.name = t.typeinfo.array.primitive_type;
                }
                break;
                
                default:
                    t.name = TYPE_ERROR;
                break;
            }
        }
        break;
        case 1: // expr -> num | rnum | true | false
        {
            // printf("the term is %s\n", terminal_string[expr_node->leftmost_child->sym.t]);
            switch(expr_node->leftmost_child->sym.t){
                case NUM:
                    t.name = INTEGER;
                break;
                
                case RNUM:
                    t.name = REAL;
                break;
                
                case TRUE:
                case FALSE:
                    t.name = BOOLEAN;
                break;
                
                default:
                    t.name = TYPE_ERROR;
                break;
            }
        }
        break;
        
        case 2:     // expr -> id epsilon | id <index> | unary_op expr
        {
            if(expr_node->rightmost_child->sym.is_terminal == true){       // id | id <index> | num | rnum

                type *type_ptr = (type*)key_search_recursive(sym_tab_ptr, expr_node->leftmost_child->token.id.str, expr_node->encl_fun_type_ptr, NULL);
                if(type_ptr == NULL){
                    /**
                     * @brief It might be that here is num/rnum
                     */
                    tree_node *var_node = expr_node->rightmost_child;
                     printf("var_ndoe->token.name = %s\n", terminal_string[var_node->token.name]); 
                    if(var_node->token.name == NUM || var_node->token.name == RNUM){
                        t = get_expr_type(var_node, sym_tab_ptr);
                    }
                    else{
                        t.name = TYPE_ERROR;
                    }                                        
                }
                else{                    
                    if(type_ptr->name == ARRAY){    
                        if(expr_node->leftmost_child->sibling->sym.t != EPSILON)  // finding type of a[smth] , return primitive type of array
                            t.name = type_ptr->typeinfo.array.primitive_type;
                        else
                            t = *type_ptr;
                    }
                    else{
                        t.name = type_ptr->name;
                    }
                }

            }
            else{   // unaryop expr with +/- id | id [smth]
                t = get_expr_type(expr_node->rightmost_child, sym_tab_ptr);
                int line_num = 0;
                tree_node *tmp = expr_node->rightmost_child;
                while(tmp){
                    if(tmp->sym.is_terminal){
                        line_num = tmp->token.line_no;
                    }
                    tmp = tmp->leftmost_child;
                }
                if(t.name != INTEGER && t.name != REAL){
                    char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(err_type, "%d) SEMANTIC ERROR", line_num);
                    print_error(err_type, "Unary expression should have int/real on rhs of unaryop\n");
                    t.name = TYPE_ERROR;
                }
                else{
                     printf("The expression is unary arithmetic one, and type name is %s\n", terminal_string[t.name]); 
                }                
            }
        }
        break;
        
        default:
        {
            //  printf(">2 nodes\n"); 
            tree_node *operand1 = expr_node->leftmost_child;
            tree_node *operand2 = NULL, *operator_node = NULL;
            type t1 = get_expr_type(operand1, sym_tab_ptr), t2;
            operator op;

            while(operand1->sibling != NULL){
                operator_node = operand1->sibling;
                if(operand1->leftmost_child && operand1->leftmost_child->sym.t == ID && operand1 != operand2)
                     printf("t1 corresponds to %s at line_no %d\n", operand1->leftmost_child->token.id.str, operand1->leftmost_child->token.line_no); 
                op = get_operator(operator_node);
                operand2 = operator_node->sibling;
                
                if(operand2->leftmost_child && operand2->leftmost_child->sym.t == ID)
                     printf("t2 corresponds to %s at line_no %d\n", operand2->leftmost_child->token.id.str, operand2->leftmost_child->token.line_no); 
                
                t2 = get_expr_type(operand2, sym_tab_ptr);

                t1 = get_EopE_type(t1, op, t2);

                operand1 = operand2;
            }
            t = t1;
        }
        break;
    }
    //  printf("get_expr_type returns : %s\n", terminal_string[t.name]); 
    return t;
}

type *retreive_type(tree_node *node){
    type *type_ptr = (type*) malloc( sizeof(type) );
    
    token_name prim_type_name;
    bool basic_type = true;     // is this type a basic type (int/real/bool)
    bool is_dynamic_arr = false;
    int num_elems = 1;

    if(node->sym.is_terminal == false)   //non-terminal, i.e. NT_ARRAY
    {
        basic_type = false;
        type_ptr->name = ARRAY; // types include only real, bool, int, Array
        type_ptr->typeinfo.array.primitive_type = node->rightmost_child->sym.t;        
        prim_type_name = type_ptr->typeinfo.array.primitive_type;

        tree_node *range_node = node->leftmost_child;
        tree_node *low_range_node = range_node->leftmost_child;
        tree_node *high_range_node = range_node->rightmost_child;

        if( low_range_node->sym.t == NUM ){
            type_ptr->typeinfo.array.range_low.value = low_range_node->token.num;
            type_ptr->typeinfo.array.is_dynamic.range_low = false;
        }
        else{
            strcpy(type_ptr->typeinfo.array.range_low.lexeme, low_range_node->token.id.str);
            type_ptr->typeinfo.array.is_dynamic.range_low = true;
            is_dynamic_arr = true;        
        }

        if( high_range_node->sym.t == NUM ){
            type_ptr->typeinfo.array.range_high.value = high_range_node->token.num;
            type_ptr->typeinfo.array.is_dynamic.range_high= false;
        }
        else{
            strcpy(type_ptr->typeinfo.array.range_high.lexeme, high_range_node->token.id.str);
            type_ptr->typeinfo.array.is_dynamic.range_high= true;            
            is_dynamic_arr = true;
        }         

        if(!is_dynamic_arr)
            num_elems = type_ptr->typeinfo.array.range_high.value - type_ptr->typeinfo.array.range_low.value + 1;            

    }
    else{
        type_ptr->name = node->sym.t;        
        prim_type_name = type_ptr->name;
    }

    if(basic_type || (is_dynamic_arr == false) ){
        switch(prim_type_name){
            case INTEGER:
                type_ptr->width = WIDTH_INTEGER * num_elems;
                break;
            case REAL:
                type_ptr->width = WIDTH_REAL * num_elems;
                break;
            case BOOLEAN:
                type_ptr->width = WIDTH_BOOLEAN * num_elems;
                break;
        }
        if(!is_dynamic_arr && !basic_type)
            type_ptr->width += WIDTH_POINTER;   // An array name will also have to store the base address of first array element
    }
    else{
        type_ptr->width = WIDTH_POINTER;
    }

    type_ptr->is_assigned = false;
    return type_ptr;
}

void insert_param_in_list(params_list *list, type *t, char *param_name){
    params_list_node *param_node = (params_list_node*) malloc( sizeof(params_list_node) );
    param_node->t = t;
    strcpy( param_node->param_name, param_name);
    param_node->next = NULL;

    if(list->first == NULL){                
        list->first = param_node;
        list->last = param_node;
    }

    else{
        list->last->next = param_node;
        list->last = param_node;
    }
    list->length++;
}

void insert_function_definition(struct symbol_table_wrapper *table,char *lexeme, tree_node *inp_par_node_list, tree_node *outp_par_node_list){
    //  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
    type *existing_entry = (type *)search_hash_table_ptr_val(root_sym_tab_ptr->table, lexeme);    
    type *t = (type*) malloc( sizeof(type) );
    t->name = MODULE;
    if(existing_entry == NULL){       // => a declaration already is not existing in sym table
        t->typeinfo.module.is_declared = false;            
        t->typeinfo.module.is_declrn_valid = false;        
    }
    else{
        t->typeinfo.module.is_declared = true;
        t->typeinfo.module.is_declrn_valid = existing_entry->typeinfo.module.is_declrn_valid;
    }

     printf("||||||||||||||||||||||||Marked declrn invalid for %s||||||||||||||||||||||\n", lexeme); 
    
    t->typeinfo.module.is_defined = true;
    strcpy(t->typeinfo.module.module_name, lexeme);
    t->typeinfo.module.input_params = (params_list*) malloc( sizeof(params_list));
    t->typeinfo.module.input_params->first = NULL;
    t->typeinfo.module.input_params->last = NULL;
    t->typeinfo.module.input_params->length = 0;

    t->typeinfo.module.output_params = (params_list*) malloc( sizeof(params_list));
    t->typeinfo.module.output_params->first = NULL;
    t->typeinfo.module.output_params->last = NULL;
    t->typeinfo.module.output_params->length = 0;
    t->typeinfo.module.base_addr = 0;
    t->typeinfo.module.curr_offset = 0;
    t->width = DONT_CARE;
    t->offset = DONT_CARE;
    
    if(inp_par_node_list != NULL){      // It will be NULL if function does not accepts any input, bcz then input_plist will be epsilon node and it's leftmost child = NULL
        char *id_str = inp_par_node_list->token.id.str;
        tree_node *inp_param_node = inp_par_node_list->sibling;  // It will always exist bcz atleast one input if above is not NULL and this list contain even #elems ID->TYPE->ID->TYPE...
        type *inp_param_type_ptr = NULL;
        while(inp_param_node != NULL){
            inp_param_type_ptr = retreive_type(inp_param_node);
            inp_param_type_ptr->offset = t->typeinfo.module.curr_offset;
            t->typeinfo.module.curr_offset += inp_param_type_ptr->width;
            insert_param_in_list(t->typeinfo.module.input_params, inp_param_type_ptr, id_str);
            inp_param_node = inp_param_node->sibling;     // rn at node labelled ID
            id_str = inp_param_node->token.id.str;
            if(inp_param_node)
                inp_param_node = inp_param_node->sibling; // goto corresponding type which is ur sibling
        }
    }

    if(outp_par_node_list != NULL){      // It will be NULL if function does not returns any output, bcz then output_plist will be epsilon node and it's leftmost child = NULL
        char *id_str = outp_par_node_list->token.id.str;
        tree_node *outp_param_node = outp_par_node_list->sibling;  // It will always exist bcz atleast one output if above is not NULL and this list contain even #elems ID->TYPE->ID->TYPE...
        type *outp_param_type_ptr = NULL;
        while(outp_param_node != NULL){
            outp_param_type_ptr = retreive_type(outp_param_node);
            
            outp_param_type_ptr->offset = t->typeinfo.module.curr_offset;
            t->typeinfo.module.curr_offset += outp_param_type_ptr->width;

            insert_param_in_list(t->typeinfo.module.output_params, outp_param_type_ptr, id_str);
            outp_param_node = outp_param_node->sibling;     // rn at node labelled ID
            id_str = outp_param_node->token.id.str;
            if(outp_param_node)
                outp_param_node = outp_param_node->sibling; // goto corresponding type which is ur sibling
        }
    }

    hash_insert_ptr_val(table->table, lexeme, t);
}

bool is_declaring_node(tree_node *node){
    //  printf("is decring node fn called for %s\n", non_terminal_string[node->sym.nt]); 
    if(node == NULL)
        return false;
    
    switch(node->sym.nt){
        case MODULEDECLARATIONS:
            // case INPUT_PLIST:    // removed bcz function arguements are inserted in parent table only and
            // case OUTPUT_PLIST:   // not in current scope to allow shadowing of variables
        // case FORLOOP:    //removed because iterating variable must be declared before for loop
            return true;
        break;
        case IDLIST:
            if(node->parent && node->parent->sym.nt == DECLARESTMT)
                return true;
            else
                return false;
        break;
        default:
            return false;
        break;
    }
}

void insert_in_sym_table(struct symbol_table_wrapper *sym_table,tree_node *node){
    type *type_ptr = (type*) malloc( sizeof(type) );    
    if(node == NULL || node->parent == NULL)
        return;
    
    switch(node->parent->sym.nt){
        case MODULEDECLARATIONS:      
             printf("Was inside moddecs\n"); 
            type_ptr->name = MODULE;
            type_ptr->typeinfo.module.is_declared = true;
            type_ptr->typeinfo.module.is_defined = false;
            type_ptr->typeinfo.module.is_declrn_valid = false;
             printf("||||||||||||||||||||||||Marked declrn invalid for %s||||||||||||||||||||||\n", node->token.id.str); 
            strcpy(type_ptr->typeinfo.module.module_name, node->token.id.str);
            type_ptr->typeinfo.module.input_params = NULL;
            type_ptr->typeinfo.module.output_params = NULL;
            type_ptr->typeinfo.module.curr_offset = 0;
            type_ptr->offset = DONT_CARE;
            type_ptr->width = DONT_CARE;
            break;
        
        // case INPUT_PLIST:
        // case OUTPUT_PLIST:
        //     type_ptr = retreive_type(node->sibling);
        //     break;
        
        case IDLIST:
            if(node->parent && node->parent->parent && node->parent->parent->sym.nt == DECLARESTMT){
                free(type_ptr);
                type_ptr = retreive_type(node->parent->sibling);
                type_ptr->offset = node->encl_fun_type_ptr->typeinfo.module.curr_offset;
                node->encl_fun_type_ptr->typeinfo.module.curr_offset += type_ptr->width;
            }
            else{
                return;
            }
            break;
        default:
            return;
            break;
    }
    type_ptr->is_assigned = false;
    hash_insert_ptr_val(sym_table->table, node->token.id.str, type_ptr);
}

void print_params_list(params_list *list){
    if(list == NULL){
         printf("[0]"); 
        return;
    }
    params_list_node *type_tmp = list->first;
     printf("[%d] - ", list->length); 
    while(type_tmp != NULL){
         printf("{%s}%s",type_tmp->param_name, terminal_string[type_tmp->t->name]); 
        if(type_tmp->t->name == ARRAY){            
            if(type_tmp->t->typeinfo.array.is_dynamic.range_low){
                 printf(" Dynamic left range | [%s .. ", type_tmp->t->typeinfo.array.range_low.lexeme); 
            }
            else{
                 printf(" Static left range | [%d .. ", type_tmp->t->typeinfo.array.range_low.value); 
            }
            if(type_tmp->t->typeinfo.array.is_dynamic.range_high){
                 printf("%s] Dynamic right range | ", type_tmp->t->typeinfo.array.range_high.lexeme); 
            }
            else{
                 printf("%d] Static right range | ", type_tmp->t->typeinfo.array.range_high.value); 
            }
             printf("Prim Type : %s",  terminal_string[type_tmp->t->typeinfo.array.primitive_type]); 
        }
         printf(" | "); 
        type_tmp = type_tmp->next;
    }

}

void print_a_type(type *type_ptr){
    if(type_ptr == NULL)
        return;
    printf("Width : %d | ", type_ptr->width); 
    printf("Offset : %d | ", type_ptr->offset); 
    printf("Type : %s | ",terminal_string[ type_ptr->name] ); 
    
    if(type_ptr->name == ARRAY){
          printf("Prim_type : %s ",terminal_string[type_ptr->typeinfo.array.primitive_type] ); 
        if(type_ptr->typeinfo.array.is_dynamic.range_low){
              printf(" Dynamic left range | [%s .. ", type_ptr->typeinfo.array.range_low.lexeme); 
        }
        else{
              printf(" Static left range | [%d .. ", type_ptr->typeinfo.array.range_low.value); 
        }
        if(type_ptr->typeinfo.array.is_dynamic.range_high){
              printf("%s] Dynamic right range | ", type_ptr->typeinfo.array.range_high.lexeme); 
        }
        else{
              printf("%d] Static right range | ", type_ptr->typeinfo.array.range_high.value); 
        }
        //   printf("\n"); 
    }

    else if(type_ptr->name == MODULE){
          printf("\n"); 
          printf("Module name : %s\n", type_ptr->typeinfo.module.module_name); 
        print_params_list(type_ptr->typeinfo.module.input_params);
        print_params_list(type_ptr->typeinfo.module.output_params);
          printf("is_declared : %d\n",type_ptr->typeinfo.module.is_declared);             
          printf("is_defined : %d\n",type_ptr->typeinfo.module.is_defined);             
    }
}

void print_symbol_table(struct symbol_table_wrapper *sym_tab_ptr){
    if(sym_tab_ptr == NULL){
        printf("Empty Symbol table\n");    
        return;
    }
     printf("************************Printing Symbol table for a new scope**********************\n"); 
    for(int i=0; i < HASH_SIZE; i++){
        type *type_ptr = (type*)(sym_tab_ptr->table[i].value);
        if(type_ptr != NULL)
        {
             printf("%s | ",sym_tab_ptr->table[i].lexeme); 
             printf("Width : %d | ", type_ptr->width); 
             printf("Offset : %d | ", type_ptr->offset); 
             printf("Type : %s | ",terminal_string[ type_ptr->name] ); 
            
            if(type_ptr->name == ARRAY){
                 printf("Prim_type : %s ",terminal_string[type_ptr->typeinfo.array.primitive_type] ); 
                if(type_ptr->typeinfo.array.is_dynamic.range_low){
                     printf(" Dynamic left range | [%s .. ", type_ptr->typeinfo.array.range_low.lexeme); 
                }
                else{
                     printf(" Static left range | [%d .. ", type_ptr->typeinfo.array.range_low.value); 
                }
                if(type_ptr->typeinfo.array.is_dynamic.range_high){
                     printf("%s] Dynamic right range | ", type_ptr->typeinfo.array.range_high.lexeme); 
                }
                else{
                     printf("%d] Static right range | ", type_ptr->typeinfo.array.range_high.value); 
                }
                 printf("\n"); 
            }

            else if(type_ptr->name == MODULE){
                 printf("\n"); 
                 printf("Module name : %s\n", type_ptr->typeinfo.module.module_name); 
                print_params_list(type_ptr->typeinfo.module.input_params);
                print_params_list(type_ptr->typeinfo.module.output_params);
                 printf("is_declared : %d\n",type_ptr->typeinfo.module.is_declared);             
                 printf("is_defined : %d\n",type_ptr->typeinfo.module.is_defined);             
            }
             printf("\n------------------------------------------------\n"); 
        }
    }
    if(sym_tab_ptr->leftmost_child_table){
         printf("\t\t\t\t Leftmost child\n"); 
        print_symbol_table(sym_tab_ptr->leftmost_child_table);
    }
     printf("************************Printing Symbol table for this scope ends**********************\n"); 
    if(sym_tab_ptr->sibling_table){
         printf("\t\t\t\t Sibling\n"); 
        print_symbol_table(sym_tab_ptr->sibling_table);
    }
    
}

void print_symbol_(tree_node *temp) {        
    if(!temp)
        return;
  if (temp->sym.is_terminal == true) {	
    printf("%s\n", terminal_string[temp->sym.t]);	
  } else {	
   printf("%s\n", non_terminal_string[temp->sym.nt]);	
  }	
}

void arrindex_type_n_bounds_check(tree_node *index_node, int lb, int ub, st_wrapper *curr_sym_tab_ptr)
{
    if(index_node != NULL){
        if(index_node->token.name != NUM){
            if(index_node->token.name != ID){
                /**
                 * @brief Invalid index, neither num, nor id
                 * 
                 */
                char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                sprintf(msg, "INVALID ARRAY ACCESS(%s)", index_node->token.id.str);
                
                char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                sprintf(err_type, "%d) SEMANTIC ERROR", index_node->token.line_no);

                print_error(err_type, msg);
            }
            else{
                /**
                 * @brief Index is an identifier, search for it's type and verify it's of int type
                 * 
                 */
                type *index_type_ptr = (type*)key_search_recursive(curr_sym_tab_ptr, index_node->token.id.str, index_node->encl_fun_type_ptr, NULL);
                if(index_type_ptr){
                    type index_id_type = *index_type_ptr;
                    if(index_id_type.name != INTEGER){
                        char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(msg, "ARRAY INDEX(%s) not integer", index_node->token.id.str);
                        
                        char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(err_type, "%d) SEMANTIC ERROR", index_node->token.line_no);

                        print_error(err_type, msg);
                    }
                }                                    
            }
        }else{
            /**
             * @brief Index is an int, do bounds checking
             * 
             */
            int index = index_node->token.num;
            /**
             * @brief Index is not in bounds, flag an error
             * 
             */
            if(!(index >= lb && index <= ub) ){
                char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                sprintf(msg, "ARRAY INDEX(%d) OUT OF BOUNDS", index_node->token.num);
                
                char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                /**
                 * @brief array identifier is left sibling of index_node or leftmost child of assign_node
                 * 
                 */
                sprintf(err_type, "%d) SEMANTIC ERROR", index_node->token.line_no);

                print_error(err_type, msg);
            }
        }
    }
}

void verify_assignment_semantics(tree_node *assign_node, st_wrapper *curr_sym_tab_ptr){    
    if(assign_node == NULL)
        return;
    type rhs_type = get_expr_type(assign_node->rightmost_child, curr_sym_tab_ptr);
    type lhs_type;
    tree_node *id_node = assign_node->leftmost_child;

    /**
     * @brief Check if assigning is done to a loop variable, report an error
     */
    tree_node *forloop_node = NULL, *parent_node = assign_node;    
    
    while(parent_node->sym.nt != MAINPROGRAM){

        if(parent_node->sym.nt == FORLOOP){
            /**
             * @brief Also check that this variable is not local to current scope
             * Not required, bcz the semantic rule = for loop variable mustn't be declared
             * inside loop prevents this situation to occur
             */
            forloop_node = parent_node;
            /**
             * @brief Assignment is a part of forloop
             */
            tree_node *iter_var_node = forloop_node->leftmost_child;
            if(strcmp(iter_var_node->token.id.str, id_node->token.id.str) == 0){   // assigning to iterating variable
                char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                sprintf(msg, "Assignment to loop variable is not allowed");
                
                char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                sprintf(err_type, "%d) SEMANTIC ERROR", id_node->token.line_no);

                print_error(err_type, msg);
                
                return;
            }
        }
        parent_node = parent_node->parent;
    }

     printf("%s = .. type checking\n", id_node->token.id.str); 
    /**
     * @brief Id is leftmost child for an assignment stmt
     * Find it's type by recursively searching symbol table
     */
    bool is_outp_param = false;
    type *id_type_ptr = key_search_recursive(curr_sym_tab_ptr, id_node->token.id.str, id_node->encl_fun_type_ptr, &is_outp_param);

    if(id_type_ptr){                       
        if(get_nth_child(assign_node, 2)->sym.t == EPSILON) {
            /**
         * @brief It's of type a := smth , a maybe of primitive type or array type
         * 
         */
            lhs_type = *id_type_ptr;
        }
        else{ 
        /**
         * @brief It's of type a[smth] := smth
         * 
         */
            lhs_type.name = id_type_ptr->typeinfo.array.primitive_type;
            /**
             * @brief It's a static array, do type checking
             * 
             */
            if(id_type_ptr->typeinfo.array.is_dynamic.range_high == false && id_type_ptr->typeinfo.array.is_dynamic.range_low == false){
                tree_node *index_node = id_node->sibling;
                int lb = id_type_ptr->typeinfo.array.range_low.value;
                int ub = id_type_ptr->typeinfo.array.range_high.value;
                arrindex_type_n_bounds_check(index_node, lb, ub, curr_sym_tab_ptr);
            }
        }

         printf("LHS_TYPE : %s, RHS_TYPE : %s\n", terminal_string[lhs_type.name], terminal_string[rhs_type.name]); 
        
        bool type_err = false;
        if(lhs_type.name != rhs_type.name)
            type_err = true;
        else{
            if(lhs_type.name == ARRAY){
                if(lhs_type.typeinfo.array.primitive_type != rhs_type.typeinfo.array.primitive_type){
                    type_err = true;
                }
                else{
                    if(!lhs_type.typeinfo.array.is_dynamic.range_low && !rhs_type.typeinfo.array.is_dynamic.range_low){
                        if(lhs_type.typeinfo.array.range_low.value != rhs_type.typeinfo.array.range_low.value)
                            type_err = true;
                    }
                    if(!lhs_type.typeinfo.array.is_dynamic.range_high && !rhs_type.typeinfo.array.is_dynamic.range_high){
                        if(lhs_type.typeinfo.array.range_high.value != rhs_type.typeinfo.array.range_high.value)
                            type_err = true;
                    }
                }
            }
        }
        if(type_err){
            
            char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
            sprintf(msg, "LHS and RHS type do not match");
            
            char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
            sprintf(err_type, "%d) SEMANTIC ERROR", id_node->token.line_no);

            print_error(err_type, msg);
        }
        
        if(is_outp_param){
             printf("Markjing %s as assigned******************************\n", id_node->token.id.str); 
            mark_outp_param_assigned(id_node->token.id.str, id_node->encl_fun_type_ptr);
        }
        tree_node *while_node = create_tree_node();
        int child_dir;
        if(is_id_part_of_while_loop(id_node, &while_node, &child_dir)){
            // install_id_in_loop_args(while_node, id_node->token.id.str);
            mark_while_loop_var_assigned(while_node, id_node->token.id.str);
        }
        
        
         printf("=======Type matches=======\n");         
    }
}

void verify_switch_semantics(tree_node *switch_node, st_wrapper *curr_sym_tab_ptr){
    if(switch_node == NULL)
        return;
    tree_node *id_node = switch_node->leftmost_child;
    tree_node *cases_head = get_nth_child(switch_node, 2);
    if(cases_head->sym.nt == CASESTMTS){
        cases_head = cases_head->leftmost_child;    //multiple cases
    }
    tree_node *default_node = switch_node->rightmost_child; //is epsilon if no default stmt, else child of this node are default and stmts node
    type *type_ptr = (type*)key_search_recursive(curr_sym_tab_ptr, id_node->token.id.str, id_node->encl_fun_type_ptr, NULL);
    if(type_ptr){
        token_name switch_var_type = type_ptr->name;

        switch(switch_var_type){
            case INTEGER:
            {
                tree_node *case_node = cases_head;
                while(case_node != NULL && case_node->sym.nt != DEFAULTSTMT){
                    //  printf("Checking case value's type, got : "); 
                    if(case_node->leftmost_child->token.name != NUM){
                        //  printf("%s", terminal_string[case_node->leftmost_child->sym.t]); 
                        //  printf("\n"); 
                        char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(msg, "Here, case value must be of integer type");
                        
                        char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(err_type, "%d) SEMANTIC ERROR", case_node->leftmost_child->token.line_no);
                        print_error(err_type, msg);
                    }
                    case_node = case_node->sibling;
                }

                tree_node *default_node;
                if(case_node != NULL)
                    default_node = case_node;
                else
                    default_node = switch_node->rightmost_child;
                
                if(default_node->token.name == EPSILON){
                    char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(msg, "Switch must have a default statement here");
                    
                    char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(err_type, "%d) SEMANTIC ERROR", id_node->token.line_no);

                    print_error(err_type, msg);
                }
            }
            break;
            case BOOLEAN:
            {
                tree_node *case_node = cases_head;
                while(case_node != NULL && case_node->sym.nt != DEFAULTSTMT){
                    //  printf("Checking case value's type, got : "); 
                    if(case_node->leftmost_child->token.name != TRUE && 
                       case_node->leftmost_child->token.name != FALSE){
                        //  printf("%s", terminal_string[case_node->leftmost_child->sym.t]); 
                        //  printf("\n"); 
                        char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(msg, "Here, case value must be of boolean type");
                        
                        char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(err_type, "%d) SEMANTIC ERROR", case_node->leftmost_child->token.line_no);
                        print_error(err_type, msg);
                    }
                    case_node = case_node->sibling;
                }

                tree_node *default_node;
                if(case_node != NULL)
                    default_node = case_node;
                else
                    default_node = switch_node->rightmost_child;
                
                if(default_node->token.name != EPSILON){
                    char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(msg, "Switch must not have a default statement here");
                    
                    char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(err_type, "%d) SEMANTIC ERROR", id_node->token.line_no);

                    print_error(err_type, msg);
                }
            }
            break;
            default:
            {
                char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                sprintf(msg, "INVALID TYPE switch variable(%s), must be integer / boolean", id_node->token.id.str);
                
                char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                sprintf(err_type, "%d) SEMANTIC ERROR", id_node->token.line_no);

                print_error(err_type, msg);
            }
            break;
        }
    }
}

bool is_types_matching(type *t1, type *t2){
    if(t1 == NULL || t2 == NULL){
         printf("One of types is null\n"); 
        if(!t1)
             printf("It's the first one\n"); 
        if(!t2)
             printf("It's the second one\n"); 
        return false;
    }
    
    if(t1->name != t2->name){
         printf("Both have diff names\n"); 
        return false;   // both diff type
    }
        
    if(t1->name != ARRAY){   //both primitive type
         printf("Both are primitive types and same\n"); 
        return true;
    }
    
    // both arrray type, do prim type checking
    if(t1->typeinfo.array.primitive_type != t2->typeinfo.array.primitive_type){
         printf("Both array type and prim types don't match\n"); 
        return false;   // prim types don't match
    }
    
    if(t1->typeinfo.array.is_dynamic.range_low || t2->typeinfo.array.is_dynamic.range_low || t1->typeinfo.array.is_dynamic.range_high || t2->typeinfo.array.is_dynamic.range_high){
         printf("One of array is dynamic\n"); 
        return true;    // if either is dynamic, say true bcz runtime checking required
    }

    // range checking if not dynamic
    
    if((t1->typeinfo.array.range_low.value != t2->typeinfo.array.range_low.value) || (t1->typeinfo.array.range_high.value != t2->typeinfo.array.range_high.value)){
         printf("Both statia but ranges don't match\n"); 
        return false;   // either low or high range values don't match
    }
     printf("Both static arrays and all matches\n"); 
    return true;    // static and everything matches
}

void compare_args_list(params_type parm_type, tree_node *fncall_args_list, params_list *fndefn_params_list, type *encl_fun_type_ptr, int line_no, st_wrapper *curr_sym_tab_ptr){    
    params_list_node *fndefn_list_node = fndefn_params_list->first;
    tree_node *fncall_list_node = fncall_args_list->leftmost_child;
    
    char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);    
    char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
    char *parameters_type = (parm_type == output) ? "Output" : "Input";

    sprintf(err_type, "%d) SEMANTIC ERROR", line_no);
    if(fndefn_list_node == NULL && fncall_list_node == NULL)
        return; // both empty => fn doesn't returns anything and is called accordingly
    
    if(fndefn_list_node == NULL){
        sprintf(msg, "Too many arguements in function call statement at %s parameters", parameters_type);
        print_error(err_type, msg);
        return;
    }
    if(fncall_list_node == NULL){
        sprintf(msg, "Too few arguements in function call statement at %s parameters", parameters_type);
        print_error(err_type, msg);
        return;
    }

    // Check type one by one
    int arg_cnt = 1;
    while(fncall_list_node && fndefn_list_node){
        type *fncall_list_node_type_ptr = (type*)key_search_recursive(curr_sym_tab_ptr, fncall_list_node->token.id.str, encl_fun_type_ptr, NULL);
         printf("Checking types of %s and %s at line %d\n", fncall_list_node->token.id.str, fndefn_list_node->param_name, line_no); 
        if(is_types_matching(fncall_list_node_type_ptr, fndefn_list_node->t) == false){
            sprintf(msg, "Parameters type mismatch at param number %d for %s params", arg_cnt, parameters_type);
            print_error(err_type, msg);
        }

        fncall_list_node = fncall_list_node->sibling;
        fndefn_list_node = fndefn_list_node->next;
        //  printf("Stuck in loop?\n"); 
        arg_cnt++;
    }
    
    if(fndefn_list_node){
        sprintf(msg, "Too few arguements in function call statement at %s parameters", parameters_type);
        print_error(err_type, msg);
        return;
    }
    
    if(fncall_list_node){
        sprintf(msg, "Too many arguements in function call statement at %s parameters", parameters_type);
        print_error(err_type, msg);
        return;
    }
    
     printf("Function called properly\n\n"); 
}

void verify_fncall_semantics(tree_node *fn_call_node, st_wrapper *curr_sym_tab_ptr){
    if(fn_call_node == NULL)
        return;
    
    tree_node *fn_id_node = get_nth_child(fn_call_node, 2);

     printf("Verifying fn call semantics for %s at %d\n", fn_id_node->token.id.str, fn_id_node->token.line_no); 
    char *encl_fun_name;
    encl_fun_name = fn_call_node->encl_fun_type_ptr->typeinfo.module.module_name;
        
     printf("My fun name is %s\n", encl_fun_name); 
    char *called_fun_name = fn_id_node->token.id.str;

    if(strcmp(encl_fun_name, called_fun_name) == 0 && strcmp(encl_fun_name, "main") != 0){
        
        char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
        sprintf(err_type, "%d) SEMANTIC ERROR", fn_id_node->token.line_no);

        print_error(err_type, "Recursion not allowed");
    }

    //mark the declaration of this function valid if it is not already defined
    type *fn_sym_tab_entry = (type *)search_hash_table_ptr_val(root_sym_tab_ptr->table, called_fun_name);
    if(fn_sym_tab_entry && fn_sym_tab_entry->typeinfo.module.is_defined == false){
         printf("||||||||||||||||||||||||Marked declrn valid for %s||||||||||||||||||||||\n", called_fun_name); 
        fn_sym_tab_entry->typeinfo.module.is_declrn_valid = true;
    }

    tree_node *fncall_outp_list = fn_call_node->leftmost_child;
    tree_node *fncall_inp_list = fn_call_node->rightmost_child;    

    if(fn_sym_tab_entry != NULL && fn_sym_tab_entry->typeinfo.module.is_defined){
        params_list *fndefn_outp_params_list = fn_sym_tab_entry->typeinfo.module.output_params;
        params_list *fndefn_inp_params_list = fn_sym_tab_entry->typeinfo.module.input_params;
        // print_symbol_table(curr_sym_tab_ptr);
        compare_args_list(output, fncall_outp_list, fndefn_outp_params_list, fn_call_node->encl_fun_type_ptr, fn_id_node->token.line_no, curr_sym_tab_ptr);
        compare_args_list(input, fncall_inp_list, fndefn_inp_params_list, fn_call_node->encl_fun_type_ptr, fn_id_node->token.line_no, curr_sym_tab_ptr);
    }    
    
    // mark each lhs param to be assigned if it's an output param for enclosing function
    if(fn_call_node->encl_fun_type_ptr != NULL){
        tree_node *fncall_list_node = fncall_outp_list->leftmost_child;
        bool is_outp_node = false;
        while(fncall_list_node){            
            key_search_recursive(curr_sym_tab_ptr, fncall_list_node->token.id.str, fn_call_node->encl_fun_type_ptr, &is_outp_node);
            
            if(is_outp_node)
                mark_outp_param_assigned(fncall_list_node->token.id.str, fn_call_node->encl_fun_type_ptr);
            
            tree_node *while_node = create_tree_node();
            int child_dir;
            if(is_id_part_of_while_loop(fncall_list_node, &while_node, &child_dir)){
                // install_id_in_loop_args(while_node, fncall_list_node->token.id.str);
                mark_while_loop_var_assigned(while_node, fncall_list_node->token.id.str);
            }
            
            /**
             * @brief Check if this id is part of any enclosing for loop
             */
            tree_node *forloop_node = NULL, *parent_node = fncall_list_node;    
    
            while(parent_node->sym.nt != MAINPROGRAM){

                if(parent_node->sym.nt == FORLOOP){
                    /**
                     * @brief Also check that this variable is not local to current scope
                     * Not required, bcz the semantic rule = for loop variable mustn't be declared
                     * inside loop prevents this situation to occur
                     */
                    forloop_node = parent_node;
                    /**
                     * @brief Assignment is a part of forloop
                     */
                    tree_node *iter_var_node = forloop_node->leftmost_child;
                    if(strcmp(iter_var_node->token.id.str, fncall_list_node->token.id.str) == 0){   // assigning to iterating variable
                        char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(msg, "Assignment to loop variable is not allowed");
                        
                        char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(err_type, "%d) SEMANTIC ERROR", fncall_list_node->token.line_no);

                        print_error(err_type, msg);
                        
                        return;
                    }
                }
                parent_node = parent_node->parent;
            }


            fncall_list_node = fncall_list_node->sibling;
        }
    }
}

void verify_fndefn_semantics(tree_node *node, st_wrapper *curr_sym_tab_ptr){
     printf("Verifying fndefn semantics for node"); 
    // print_symbol(node->sym);
     printf("\n"); 
    
    tree_node *outp_list_node = get_nth_child(node, 3)->leftmost_child;
    type *outp_list_node_type_ptr = NULL;

    while(outp_list_node != NULL){
        outp_list_node_type_ptr = (type*) key_search_recursive(curr_sym_tab_ptr, outp_list_node->token.id.str, node->encl_fun_type_ptr, NULL);

        if(outp_list_node_type_ptr->is_assigned == false){
            char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
            sprintf(msg, " Output parameter(%s) not assigned value for function defn of %s", outp_list_node->token.id.str, node->leftmost_child->token.id.str);
            
            char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
            sprintf(err_type, "%d) SEMANTIC ERROR", outp_list_node->token.line_no);

            print_error(err_type, msg);
        }

        outp_list_node = outp_list_node->sibling;

        if(outp_list_node)
            outp_list_node = outp_list_node->sibling;
    }

}

void verify_declarations_validity(tree_node *mainprog_node){
    tree_node *mod_dec_id_node = mainprog_node->leftmost_child->leftmost_child;
    type *mod_id_sym_tab_entry = NULL;

    while(mod_dec_id_node != NULL){
         printf("inside verify_declrns_validity, searching sym tab for %s\n", mod_dec_id_node->token.id.str); 
        mod_id_sym_tab_entry = (type *)search_hash_table_ptr_val(root_sym_tab_ptr->table, mod_dec_id_node->token.id.str);

        if(mod_id_sym_tab_entry && mod_id_sym_tab_entry->typeinfo.module.is_declrn_valid == false && mod_id_sym_tab_entry->typeinfo.module.is_defined == true){
             printf("||||||||||||||||||||||||Checking declrn validity for %s||||||||||||||||||||||\n", mod_dec_id_node->token.id.str); 
            char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
            sprintf(err_type, "%d) SEMANTIC ERROR", mod_dec_id_node->token.line_no);

            print_error(err_type, "Module declaration is redundant");
        }

        mod_dec_id_node = mod_dec_id_node->sibling;
    }


}

void mark_while_loop_var_assigned(tree_node *while_node, char *var_name){
     printf("Will be marking loop variable %s as assigned\n", var_name); 
    int num_vars = 0;
    void *loop_args = while_node->extra_args;
    if(loop_args)
    {
        num_vars = *(int*)loop_args;    
        loop_args = loop_args + sizeof(int);
        
        int id_size = sizeof(char)*MAX_LEXEME_LEN;
        int entry_size = sizeof(bool) + id_size;
         printf("NUM_VARS = %d\n", num_vars); 
        for(int i = 0; i < num_vars; i++)
        {
             printf("loop_args_str[%d] = %s\n", i, (char*)loop_args); 
            if(strcmp((char*)(loop_args), var_name) == 0){
                *(bool*)(loop_args + id_size) = true;
                 printf("And yooooooooo, marked successfully\n"); 
            }                
            loop_args = loop_args + entry_size;
        }
    }
    else{
         printf("LoLoL!!!, loop_args dne\n"); 
    }
}

bool is_id_part_of_while_loop(tree_node *id_node, tree_node **while_node, int *child_dir){
     printf("Checking is id part of while loop for id = %s\n", id_node->token.id.str); 
    while(id_node){
        if(id_node->parent){
            //  printf("id_node's parent exists and is %s\n", non_terminal_string[id_node->parent->sym.nt]); 
            if(id_node->parent->sym.nt == WHILELOOP){           
                /**
                 * @brief Also, make sure that this id is not part of current scope's symbol table
                 * This is to avoid problems where a new identifier is declared in a new scope
                 */
                *while_node = id_node->parent;
                
                if((*while_node)->leftmost_child == id_node)
                    *child_dir = WHILE_LHS;
                else
                    *child_dir = WHILE_RHS;

                 printf("And yes it is\n"); 
                return true;
            }
        }
        id_node = id_node->parent;
    }
    return false;
}

void install_id_in_loop_args(tree_node *while_node, char *id_str)
{
     printf("Will be installing %s in while_loop\n", id_str); 
    void *loop_args = while_node->extra_args;
    int id_size = sizeof(char) * MAX_LEXEME_LEN;
    int entry_size = id_size + sizeof(bool);
    int num_vars = 0;

    if(loop_args == NULL){
         printf("Initializing while_loop extra_args\n"); 
        while_node->extra_args = malloc(entry_size + sizeof(int));

        *(int *)while_node->extra_args = 1;        
        loop_args = while_node->extra_args + sizeof(int);
         printf("Initialization successful, num_vars = %d\n", *(int*)(while_node->extra_args)); 
    }    
    else{
        loop_args = while_node->extra_args;

        num_vars = *(int *)loop_args;

        loop_args = loop_args + sizeof(int);
        
         printf("Num of vars already present = %d\n", num_vars); 
        // loop_args = loop_args + entry_size*num_vars;
        for(int i = 0; i < num_vars; i++)
        {
            if(strcmp((char*)(loop_args), id_str) == 0)
                return;
            loop_args = loop_args + entry_size;
        }
         printf("Reallocating while_loop extra_args\n"); 
        while_node->extra_args = realloc(while_node->extra_args, sizeof(int) + ((num_vars + 1) * entry_size));
         printf("After reallcoation, num_vars = %d\n", *(int*)(while_node->extra_args)); 
        
        (*(int*)(while_node->extra_args))++;
        
        loop_args = while_node->extra_args + sizeof(int);

         printf("After reallcoation and updation, num_vars = %d\n", *(int*)(while_node->extra_args)); 
    }

    loop_args = loop_args + (entry_size*num_vars);

    strcpy((char*)loop_args, id_str);
     printf("installed %s???\n", (char*)loop_args); 
    loop_args = loop_args + id_size;
    *(bool*)loop_args = false;
     printf("Mark assigned? %d\n", *(bool*)loop_args); 
}

void verify_whileloop_semantics(tree_node *while_node, st_wrapper *curr_sym_tab_ptr){
     printf("Verifying while_loop semantics\n\n"); 

    type expr_type = get_expr_type(while_node->leftmost_child, curr_sym_tab_ptr);
    int line_num = 1;
    tree_node *tmp = while_node->leftmost_child;
    while(tmp){
        if(tmp->sym.is_terminal){
            if(tmp->sym.t == ID){
                line_num = tmp->token.line_no;
            }
            break;
        }
        tmp = tmp->leftmost_child;
    }
    if(expr_type.name != BOOLEAN){        
         printf("##################Invalid expression type for while expression at line %d#############\n", line_num); 
    }

    void *loop_vars = while_node->extra_args;
    int num_vars = 0;
    if(loop_vars != NULL){
         printf("Loop vars aren't NULL\n"); 
        num_vars = *(int *)loop_vars;
        loop_vars = loop_vars + sizeof(int);
        bool is_any1_assigned = false;
         printf("Num of loop variables here are %d\n", num_vars); 
        for(int i =0; i < num_vars; i++){
             printf("Checking for %s\n", (char *)loop_vars); 
            loop_vars = loop_vars + sizeof(char)*MAX_LEXEME_LEN;
            is_any1_assigned = *(bool *)loop_vars;
            if(is_any1_assigned){
                 printf("And this one is assingned\n"); 
                break;
            }                
            else{
                 printf("This one isn't assigned\n"); 
            }
            loop_vars = loop_vars + sizeof(bool);
        }
        if(is_any1_assigned == false){
            char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
            sprintf(err_type, "%d) SEMANTIC ERROR", line_num);

            print_error(err_type, "While loop, no variable involved in expression is changing...\n");
        }
    }
    else{
        char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
        sprintf(err_type, "%d) SEMANTIC ERROR", line_num);

        print_error(err_type, "While loop, no variable involved in expression is changing...\n");
    }
}

void verify_construct_semantics(tree_node *node){
    switch(node->sym.nt){
        case CONDITIONALSTMT:
        {
            /**
             * @brief Have visited all children of switch, 
             * check if semantics hold and flag errors accordingly
             */
            verify_switch_semantics(node, curr_sym_tab_ptr);
        }
        break;
        case ASSIGNMENTSTMT:
        {
            verify_assignment_semantics(node, curr_sym_tab_ptr);
        }
        break;
        case FUNCTIONCALLSTMT:
        {            
            verify_fncall_semantics(node, curr_sym_tab_ptr);
        }
        break;
        case IOSTMT:
        {
            tree_node *id_node = NULL;
            if(node->leftmost_child->sym.t == GET_VALUE){
                id_node = node->rightmost_child;
                bool is_outp_node = false;
                key_search_recursive(curr_sym_tab_ptr, id_node->token.id.str, id_node->encl_fun_type_ptr, &is_outp_node);
                
                if(is_outp_node)
                    mark_outp_param_assigned(id_node->token.id.str, id_node->encl_fun_type_ptr);
                
                tree_node *while_node = create_tree_node();
                int child_dir;
                if(is_id_part_of_while_loop(id_node, &while_node, &child_dir)){
                    // install_id_in_loop_args(while_node, id_node->token.id.str);
                    mark_while_loop_var_assigned(while_node, id_node->token.id.str);
                }

                /**
                 * @brief Check if thhis id is part of any enclosing for loop
                 */
                tree_node *forloop_node = NULL, *parent_node = node;    
        
                while(parent_node->sym.nt != MAINPROGRAM){

                    if(parent_node->sym.nt == FORLOOP){
                        /**
                         * @brief Also check that this variable is not local to current scope
                         * Not required, bcz the semantic rule = for loop variable mustn't be declared
                         * inside loop prevents this situation to occur
                         */
                        forloop_node = parent_node;
                        /**
                         * @brief Assignment is a part of forloop
                         */
                        tree_node *iter_var_node = forloop_node->leftmost_child;
                        if(strcmp(iter_var_node->token.id.str, id_node->token.id.str) == 0){   // assigning to iterating variable
                            char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                            sprintf(msg, "Assignment to loop variable is not allowed");
                            
                            char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                            sprintf(err_type, "%d) SEMANTIC ERROR", id_node->token.line_no);

                            print_error(err_type, msg);
                            
                            return;
                        }
                    }
                    parent_node = parent_node->parent;
                }
            }
        }
        break;
        case WHILELOOP:
        {
            verify_whileloop_semantics(node, curr_sym_tab_ptr);            
        }
        break;
        case FORLOOP:
        {
            // check iterating variable type
            tree_node *id_node = node->leftmost_child;
            type *id_type = (type*)key_search_recursive(curr_sym_tab_ptr, id_node->token.id.str, node->encl_fun_type_ptr, NULL);
            if(id_type){
                if(id_type->name != INTEGER){
                    char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(err_type, "%d) SEMANTIC ERROR", id_node->token.line_no);
                    print_error(err_type, "Loop variable must be integral type");
                }
            }
        }
        break;
        case NTMODULE:
        {
            verify_fndefn_semantics(node, curr_sym_tab_ptr);
        }
        break;
        case MAINPROGRAM:
        {
            verify_declarations_validity(node);
        }
        break;
        case NT_ARRAY:
        {
            tree_node *range_node = node->leftmost_child;
            tree_node *low_range_node = range_node->leftmost_child;
            tree_node *high_range_node = range_node->rightmost_child;
            if(low_range_node->sym.t == ID){
                type *low_range_type = (type *)key_search_recursive(curr_sym_tab_ptr, low_range_node->token.id.str, node->encl_fun_type_ptr, NULL);
                if(low_range_type->name != INTEGER){
                    char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(msg, "Range %s must be integer type", low_range_node->token.id.str);
                    
                    char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(err_type, "%d) SEMANTIC ERROR", low_range_node->token.line_no);

                    print_error(err_type, msg);
                }
            }

            if(high_range_node->sym.t == ID){
                type *high_range_type = (type *)key_search_recursive(curr_sym_tab_ptr, high_range_node->token.id.str, node->encl_fun_type_ptr, NULL);
                if(high_range_type->name != INTEGER){
                    char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(msg, "Range %s must be integer type", high_range_node->token.id.str);
                    
                    char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                    sprintf(err_type, "%d) SEMANTIC ERROR", high_range_node->token.line_no);

                    print_error(err_type, msg);
                }
            }
        }
        break;
    }
}

type* check_encl_fun_params(type *fun_type, char *lexeme, bool *is_outp_param)
{
    if(fun_type == NULL){
        return NULL;
    }
    params_list_node *inp_param = NULL; 
    if(fun_type->typeinfo.module.input_params) 
        inp_param = fun_type->typeinfo.module.input_params->first;
    while(inp_param != NULL){
        if(strcmp(inp_param->param_name, lexeme) == 0){
            //  printf("%s is a input param\n", lexeme); 
            return inp_param->t;
        }
        inp_param = inp_param->next;
    }

    params_list_node *outp_param = NULL;
    if(fun_type && fun_type->typeinfo.module.output_params)
        outp_param = fun_type->typeinfo.module.output_params->first;
    
    while(outp_param != NULL){
        if(strcmp(outp_param->param_name, lexeme) == 0){
            //  printf("%s is a output param\n", lexeme); 
            if(is_outp_param != NULL)
                *is_outp_param = true;
            return outp_param->t;
        }
        outp_param = outp_param->next;
    }

    return NULL;
}

void mark_outp_param_assigned(char *id_str, type *fn_type_ptr){
    params_list_node *outp_param_node = NULL;

    if(fn_type_ptr && fn_type_ptr->typeinfo.module.output_params){
        outp_param_node = fn_type_ptr->typeinfo.module.output_params->first;
    }

    while(outp_param_node != NULL){
        if(strcmp(outp_param_node->param_name, id_str) == 0){   // found the output parameter which is to be marked
            outp_param_node->t->is_assigned = true;
        }
        outp_param_node = outp_param_node->next;
    }
}

void construct_symtable(tree_node *ast_root) {
  tree_node *node = ast_root;
    if(node == NULL)
        return;
  do{        
    //   print_symbol(node->sym);
        if(node->visited == false) {
            node->visited = true;
            /**
             * @brief It's a nt, check if it opens up a new scope, if it does create a new symbol table
             * If it's NTMODULE, install function parameters and return types in symbol table
             */
            if(node->sym.is_terminal == false){
                if(node->sym.nt == NTMODULE){
                    insert_function_definition(root_sym_tab_ptr,node->leftmost_child->token.id.str, get_nth_child(node, 2)->leftmost_child, get_nth_child(node, 3)->leftmost_child); //pass the list heads for input and output types
                    // encl fun ptr stores ptr to module's type entry
                    node->encl_fun_type_ptr = search_hash_table_ptr_val(root_sym_tab_ptr->table,node->leftmost_child->token.id.str);
                }
                else{
                    /**
                     * @brief Any non-terminal except NTMODULE and DRIVERMODULE inherits encl_fun_type_ptr value from it's parent
                     */
                    if(node->sym.nt == DRIVERMODULE){
                        type *driver_encl_ptr = (type *)malloc(sizeof(type));
                        driver_encl_ptr->name = MODULE;
                        driver_encl_ptr->is_assigned = false;
                        driver_encl_ptr->typeinfo.module.input_params = NULL;
                        driver_encl_ptr->typeinfo.module.is_declared = false;
                        driver_encl_ptr->typeinfo.module.is_declrn_valid = false;
                        driver_encl_ptr->typeinfo.module.is_defined = true;
                        driver_encl_ptr->typeinfo.module.curr_offset = 0;
                        strcpy(driver_encl_ptr->typeinfo.module.module_name, "main");
                        driver_encl_ptr->typeinfo.module.output_params = NULL;
                        node->encl_fun_type_ptr = driver_encl_ptr;
                    }
                    else if(node->parent)
                        node->encl_fun_type_ptr = node->parent->encl_fun_type_ptr;
                }
                if(is_new_scope_openable(node->sym.nt) == true){
                    st_wrapper *new_sym_tab_ptr = symbol_table_init();

                    init_hash_table(new_sym_tab_ptr->table);
                    /**
                     * @brief Insert new symbol table in parent's children list
                     * 
                     */                    
                    if(curr_sym_tab_ptr){
                         printf("Insert new symbol table in parent's children list\n"); 
                        if(curr_sym_tab_ptr->leftmost_child_table==NULL) {
                            //  printf("as leftmost child\n"); 
                            curr_sym_tab_ptr->leftmost_child_table = new_sym_tab_ptr;
                            curr_sym_tab_ptr->rightmost_child_table = new_sym_tab_ptr; 
                        }
                        else{
                            //  printf("as rightmost child\n"); 
                            curr_sym_tab_ptr->rightmost_child_table->sibling_table = new_sym_tab_ptr;
                            curr_sym_tab_ptr->rightmost_child_table = new_sym_tab_ptr;
                        }
                    }
                    curr_sym_tab_ptr = new_sym_tab_ptr;
                     printf("\n\n New Scope Opened at %s \n\n", non_terminal_string[node->sym.nt]); 
                }
            }
            /**
             * @brief If it's ID, install it in symbol table if it's part of a declaration and not a output param
             * 
             */
            else{
                /**
                 * @brief Assign terminal's encl_fun_type_ptr to point to enclosing function's type 
                 */
                node->encl_fun_type_ptr = node->parent->encl_fun_type_ptr;
                if(node->sym.t == ID){                    
                    //  printf("Node->parent->encl_fun_type_ptr exists? %d\n", node->parent->encl_fun_type_ptr != NULL); 
                    int check_hash_table = search_hash_table(curr_sym_tab_ptr->table, node->token.id.str);
                    
                    if(is_declaring_node(node->parent)){
                        //  printf("The node is declaring : %s !!!!!!!!!!!!!!!!!\n", non_terminal_string[node->parent->sym.nt]); 
                        if( check_hash_table == KEY_NOT_FOUND ){
                            //  printf("Node->encl_fun_type_ptr exists? %d\n", node->encl_fun_type_ptr != NULL); 
                            params_list_node *output_param = NULL;
                            
                            /**
                             * @brief It will be NULL when declaration is part of driver
                             */
                            if(node->encl_fun_type_ptr && node->encl_fun_type_ptr->typeinfo.module.output_params != NULL)
                                output_param = node->encl_fun_type_ptr->typeinfo.module.output_params->first;
                            /**
                             * @brief Check if ID being assigned is an output parameter
                             */
                            bool outp_redec_err = false;
                            while(output_param != NULL){
                                if(strcmp(node->token.id.str, output_param->param_name) == 0){
                                    char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                                    sprintf(msg, "Redeclaration of output parameter %s", node->token.id.str);
                                    
                                    char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                                    sprintf(err_type, "%d) SEMANTIC ERROR", node->token.line_no);

                                    print_error(err_type, msg);
                                    outp_redec_err = true;
                                }
                                output_param = output_param->next;
                            }
                            if(!outp_redec_err)
                                insert_in_sym_table(curr_sym_tab_ptr, node);
                            
                            /**
                             * @brief Check if the id being declared is an enclosing for loop's
                             * variable, if it's report an error.
                             */
                            tree_node *parent_node = node;
                            while(parent_node->sym.nt != MAINPROGRAM){
                                if(parent_node->sym.nt == FORLOOP){
                                    tree_node *for_id_node = parent_node->leftmost_child;
                                    if(strcmp(for_id_node->token.id.str, node->token.id.str) == 0){
                                        // loop's iterating variable being declared again
                                        char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                                        sprintf(err_type, "%d) SEMANTIC ERROR", node->token.line_no);

                                        print_error(err_type, "FOR loop's iterating variable redeclared here\n");
                                    }
                                }
                                parent_node = parent_node->parent;
                            }
                        }
                        else{
                            /**
                             * @brief Id exists in current symbol table and is being declared again, flag an error
                             */                            
                            char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                            sprintf(msg, "ID(%s) declared already", node->token.id.str);
                            
                            char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                            sprintf(err_type, "%d) SEMANTIC ERROR", node->token.line_no);

                            print_error(err_type, msg);
                        }
                    }
                    else{
                        /**
                         * @brief Check if this ID is on lhs of while loop
                         * If it is, install it in loop's extra args attribute
                         */
                        int child_dir = 0;
                        tree_node *while_node = NULL;
                        if(is_id_part_of_while_loop(node, &while_node, &child_dir)){
                            if(child_dir == WHILE_LHS){
                                install_id_in_loop_args(while_node, node->token.id.str);
                            }
                        }
                    }

                }
            }

            /**
             * @brief Doing post order traversal, go to leftmost child as long as possible
             * 
             */
            if(node->leftmost_child != NULL){
                node = node->leftmost_child;
            }
    } 
        /**
         * @brief Have already visited the node, returning after visiting all children
         * 
         */
        else{                  
            /**
             * @brief Attach corresponding symbol table to this node
             */
            node->scope_sym_tab = curr_sym_tab_ptr;
            if(node->sym.is_terminal == false){ 
                //  printf("Before calling verify_construct_semantics, printing symbol table at node %s\n", non_terminal_string[node->sym.nt]); 
                // print_symbol_table(curr_sym_tab_ptr);
                verify_construct_semantics(node);                
                /**
                 * @brief If it opened up a new scope, close it          
                 */
                if(is_new_scope_openable(node->sym.nt) == true){    
                    if(curr_sym_tab_ptr)                            
                        curr_sym_tab_ptr = curr_sym_tab_ptr->parent_table;
                     printf("\n\t A scope closed for nt : %s\n", non_terminal_string[node->sym.nt]); 
                }   
            }    // if non-terminal
            /**
             * @brief It's a terminal and visited            
             */
            else{
                if(node->token.name == ID){
                /**
                 * @brief Id is being accessed              
                 */
                    //  printf("Checking for %s\n", node->token.id.str);                    
                    type *type_ptr = (type*)key_search_recursive(curr_sym_tab_ptr, node->token.id.str, node->encl_fun_type_ptr, NULL);
                                      
                    /**
                     * @brief The identifier exists in symbol table => it is declared
                     */
                    if(type_ptr){
                         printf("Received entry for %s, type_name = %s\n", node->token.id.str, terminal_string[type_ptr->name] ); 
                        if(type_ptr->name == ARRAY){
                            if(type_ptr->typeinfo.array.is_dynamic.range_low){
                                 printf(" Dynamic left range | [%s .. ", type_ptr->typeinfo.array.range_low.lexeme); 
                            }
                            else{
                                 printf(" Static left range | [%d .. ", type_ptr->typeinfo.array.range_low.value); 
                            }
                            if(type_ptr->typeinfo.array.is_dynamic.range_high){
                                 printf("%s] Dynamic right range | ", type_ptr->typeinfo.array.range_high.lexeme); 
                            }
                            else{
                                 printf("%d] Static right range | ", type_ptr->typeinfo.array.range_high.value); 
                            }
                             printf("Prim Type : %s\n",  terminal_string[type_ptr->typeinfo.array.primitive_type]);                             
                        }
                        else if(type_ptr->name == MODULE){
                             printf("Module name : %s\n", type_ptr->typeinfo.module.module_name); 
                             printf("is_declared : %d\n", type_ptr->typeinfo.module.is_declared); 
                             printf("is_decfined : %d\n", type_ptr->typeinfo.module.is_defined); 
                             printf("Input_params_list : "); 
                            print_params_list(type_ptr->typeinfo.module.input_params);
                             printf("\n"); 
                             printf("Output_params_list : "); 
                            print_params_list(type_ptr->typeinfo.module.output_params);
                             printf("\n"); 
                        }
                    }
                    else{
                        // Did not get in any parent scope, check if it's part of enclosing function's inp/outp param
                        
                        /**
                         * @brief When accessing an identifier's value and it's not declared 
                         * flag an error
                         */
                        char *msg = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(msg, "ID(%s) not declared", node->token.id.str);
                        
                        char *err_type = (char*) malloc(sizeof(char) * MAX_ERR_TYPE_STR_LEN);
                        sprintf(err_type, "%d) SEMANTIC ERROR", node->token.line_no);

                        print_error(err_type, msg);
                    }
                }
            }            
            // mark the node unvisited again to enable second ast pass
            node->visited = false;
            if (node->sibling != NULL)
                node = node->sibling;
            else 
                node = node->parent;
        } 
    }while (node != NULL);
}

void second_ast_pass(tree_node *ast_root){
    
    tree_node *ast_node = ast_root;
    while(ast_node != NULL){

        if(ast_node->visited == false){
            ast_node->visited = true;
            
            if(ast_node->sym.is_terminal == false){
                switch(ast_node->sym.nt){
                    case FUNCTIONCALLSTMT:
                    {                        
                        verify_fncall_semantics(ast_node, ast_node->scope_sym_tab);
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
            if(ast_node->sibling){
                ast_node = ast_node->sibling;
            }
            else{
                ast_node = ast_node->parent;
            }
        }

    }

}