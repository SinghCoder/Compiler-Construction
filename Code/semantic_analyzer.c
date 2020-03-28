#include "semantic_analyzer.h"
#include "semantic_analyzerDef.h"
#include "parser.h"
#include "treeADT.h"
#include <stdlib.h>
#include <string.h>

void semantic_analyzer_init(){
    curr_sym_tab.parent_table = NULL;
    init_hash_table(curr_sym_tab.table);
}

bool is_new_scope_openable(nonterminal nt){
    switch(nt){
        case MAINPROGRAM:
        case DRIVERMODULE:
        case NTMODULE:
        case CASESTMT:
        case DEFAULTSTMT:
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

type get_expr_type(tree_node *expr_node){
    int num_child = expr_node->num_child;
    type t;
    // printf("inside get expr type\n");
    // printf("    left : ");
    // print_symbol_(expr_node->leftmost_child);
    // printf("    top : ");
    // print_symbol_(expr_node);
    // printf("    right : ");
    // print_symbol_(expr_node->rightmost_child);

    switch(num_child){
        case 0:
        {
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
                
                default:
                    t.name = TYPE_ERROR;
                break;
            }
        }
        break;
        case 1: // expr -> num | rnum | true | false
        {
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
            if(expr_node->rightmost_child->sym.is_terminal == true){       // id | id <index>

                type *type_ptr = (type*)search_hash_table_ptr_val(curr_sym_tab.table, expr_node->leftmost_child->token.str);
                if(type_ptr == NULL){
                    t.name = TYPE_ERROR;
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
            else{   // unaryop expr
                t = get_expr_type(expr_node->rightmost_child);
            }
        }
        break;
        
        default:
        {
            // printf(">2 nodes\n");
            tree_node *child = expr_node->leftmost_child;
            type t1 = get_expr_type(child), t2;
            operator op;

            while(child->sibling != NULL){
                op = get_operator(child->sibling);
                t2 = get_expr_type(child->sibling->sibling);

                t1 = get_EopE_type(t1, op, t2);

                child = child->sibling->sibling;
            }
            t = t1;
        }
        break;
    }

    return t;
}

type *retreive_type(tree_node *node){
    type *type_ptr = (type*) malloc( sizeof(type) );
    if(node->sym.is_terminal == false)   //primitive type
    {
        type_ptr->name = ARRAY; // types include only real, bool, int, Array
        type_ptr->typeinfo.array.primitive_type = node->rightmost_child->sym.t;        
        
        if( node->leftmost_child->leftmost_child->sym.t == NUM ){
            type_ptr->typeinfo.array.range_low = node->leftmost_child->leftmost_child->token.num;
            type_ptr->typeinfo.array.is_dynamic = false;
        }
        else{
            type_ptr->typeinfo.array.range_low = OBTAIN_DYNAMICALLY;
            type_ptr->typeinfo.array.is_dynamic = true;
            
        }

        if( node->leftmost_child->rightmost_child->sym.t == NUM ){
            type_ptr->typeinfo.array.range_high = node->leftmost_child->rightmost_child->token.num;
            type_ptr->typeinfo.array.is_dynamic = false;
        }
        else{
            type_ptr->typeinfo.array.range_high = OBTAIN_DYNAMICALLY;
            type_ptr->typeinfo.array.is_dynamic = true;
            
        }
        
        type_ptr->typeinfo.array.range_high = ( node->leftmost_child->rightmost_child->sym.t == NUM) ? node->leftmost_child->rightmost_child->token.num : OBTAIN_DYNAMICALLY;
         
    }
    else{
        type_ptr->name = node->sym.t;
    }
    return type_ptr;
}

void insert_type_in_list(types_list *list, type *t){
    types_list_node *type_node = (types_list_node*) malloc( sizeof(types_list_node) );
    type_node->t = t;
    type_node->next = NULL;

    if(list->first == NULL){                
        list->first = type_node;
        list->last = type_node;
    }

    else{
        list->last->next = type_node;
        list->last = type_node;
    }
    list->length++;
}

void insert_function_definition(char *lexeme, tree_node *inp_par_node_list, tree_node *outp_par_node_list){
    // printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    type *t = (type*) malloc( sizeof(type) );
    t->name = MODULE;
    t->typeinfo.module.is_declared = true;
    
    t->typeinfo.module.input_types = (types_list*) malloc( sizeof(types_list));
    t->typeinfo.module.input_types->first = NULL;
    t->typeinfo.module.input_types->last = NULL;
    t->typeinfo.module.input_types->length = 0;

    t->typeinfo.module.output_types = (types_list*) malloc( sizeof(types_list));
    t->typeinfo.module.output_types->first = NULL;
    t->typeinfo.module.output_types->last = NULL;
    t->typeinfo.module.output_types->length = 0;

    if(inp_par_node_list != NULL){      // It will be NULL if function does not accepts any input, bcz then input_plist will be epsilon node and it's leftmost child = NULL
        tree_node *inp_type_node = inp_par_node_list->sibling;  // It will always exist bcz atleast one input if above is not NULL and this list contain even #elems ID->TYPE->ID->TYPE...
        while(inp_type_node != NULL){
            insert_type_in_list(t->typeinfo.module.input_types, retreive_type(inp_type_node));
            inp_type_node = inp_type_node->sibling;     // rn at node labelled ID
            if(inp_type_node)
                inp_type_node = inp_type_node->sibling; // goto corresponding type which is ur sibling
        }
    }

    if(outp_par_node_list != NULL){      // It will be NULL if function does not returns any output, bcz then output_plist will be epsilon node and it's leftmost child = NULL
        tree_node *outp_type_node = outp_par_node_list->sibling;  // It will always exist bcz atleast one output if above is not NULL and this list contain even #elems ID->TYPE->ID->TYPE...
        while(outp_type_node != NULL){
            insert_type_in_list(t->typeinfo.module.output_types, retreive_type(outp_type_node));
            outp_type_node = outp_type_node->sibling;     // rn at node labelled ID
            if(outp_type_node)
                outp_type_node = outp_type_node->sibling; // goto corresponding type which is ur sibling
        }
    }

    hash_insert_ptr_val(curr_sym_tab.table, lexeme, t);
}
void insert_in_sym_table(tree_node *node){
    type *type_ptr = (type*) malloc( sizeof(type) );    
    if(node->parent == NULL)
        return;
    switch(node->parent->sym.nt){
        case MODULEDECLARATIONS:            
            type_ptr->name = MODULE;
            type_ptr->typeinfo.module.is_declared = true;
            type_ptr->typeinfo.module.input_types = NULL;
            type_ptr->typeinfo.module.output_types = NULL;
            break;
        
        case INPUT_PLIST:
        case OUTPUT_PLIST:
            type_ptr = retreive_type(node->sibling);
            break;
        
        case IDLIST:
            if(node->parent && node->parent->parent && node->parent->parent->sym.nt == DECLARESTMT){
                free(type_ptr);
                type_ptr = retreive_type(node->parent->sibling);
            }
            else{
                return;
            }
            break;
        
        case FORLOOP:
            type_ptr->name = INTEGER;
            break;
        default:
            return;
            break;
    }

    hash_insert_ptr_val(curr_sym_tab.table, node->token.str, type_ptr);
}

void print_types_list(types_list *list){
    if(list == NULL){
        printf("[0]");
        return;
    }
    types_list_node *type_tmp = list->first;
    printf("[%d] - ", list->length);
    while(type_tmp != NULL){
        printf("%s", terminal_string[type_tmp->t->name]);
        if(type_tmp->t->name == ARRAY){
            if(type_tmp->t->typeinfo.array.is_dynamic == true){
                printf(" Dynamic indexes ");
            }
            printf("[%d...%d] of %s", type_tmp->t->typeinfo.array.range_low, type_tmp->t->typeinfo.array.range_high, terminal_string[type_tmp->t->typeinfo.array.primitive_type]);
        }
        printf(" | ");
        type_tmp = type_tmp->next;
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
void construct_symtable(tree_node *ast_root) {
  tree_node *node = ast_root;
//   printf("```````````````````````````````````````````````````````````\n");
//   print_symbol_(node);
//   printf("```````````````````````````````````````````````````````````\n");
  do{
        // printf("===================\n");
        // if(!node->visited)
        //     printf("Entered : ");
        // else
        //     printf("Exited : ");
        // print_symbol_(node);
        // printf("===================\n");
        if (node->visited == false) {
            // print_symbol_(node);
            node->visited = true;

            if(node->sym.is_terminal == false){
                if(is_new_scope_openable(node->sym.nt) == true && node->sym.nt != MAINPROGRAM){
                    st_wrapper *new_sym_tab_ptr = (st_wrapper*)malloc(sizeof(st_wrapper));
                    new_sym_tab_ptr->parent_table = &curr_sym_tab;
                    init_hash_table(new_sym_tab_ptr->table);
                    curr_sym_tab = *new_sym_tab_ptr;
                    printf("\n\n New Scope Opened at %s \n\n", non_terminal_string[node->sym.nt]);
                }
                if(node->sym.nt == NTMODULE){
                    insert_function_definition(node->leftmost_child->token.str, get_nth_child(node, 2)->leftmost_child, get_nth_child(node, 3)->leftmost_child); //pass the list heads for input and output types
                }
            }
            else{
                if(node->sym.t == ID){
                    int check_hash_table = search_hash_table(curr_sym_tab.table, node->token.str);
                    if( check_hash_table == KEY_NOT_FOUND ){
                        insert_in_sym_table(node);
                    }
                }
            }

            if(node->leftmost_child != NULL){
                node = node->leftmost_child;
                // printf("node = node->leftmost_child\n");
                // print_symbol_(node);
            }

        } 
        else{      
            if(node->sym.is_terminal == false){
                if(is_new_scope_openable(node->sym.nt) == true && node->sym.nt != MAINPROGRAM){                                
                    curr_sym_tab = *(curr_sym_tab.parent_table);
                    printf("\n\n A scope closed \n\n");
                }   
                
                /**
                 * @brief Type checking
                 */
                if(node->sym.nt == ASSIGNMENTSTMT){
                    type rhs_type = get_expr_type(node->rightmost_child);
                    type lhs_type;
                    printf("%s = .. type checking\n", node->leftmost_child->token.str);
                    
                    type *type_ptr = search_hash_table_ptr_val(curr_sym_tab.table, node->leftmost_child->token.str);
                    
                    bool arr_ind_in_range = false;
                    bool array_access = false;

                    if(!type_ptr){
                        print_error("SEMANTIC ERROR", "ID not defined");
                    }
                    else{
                        if(get_nth_child(node, 2)->sym.t == EPSILON)    // lhs is id (maybe array id or normal id)
                            lhs_type = *type_ptr;
                        
                        else{                        // lhs is array access
                            lhs_type.name = type_ptr->typeinfo.array.primitive_type;
                            if(type_ptr->typeinfo.array.is_dynamic == false){
                                array_access = true;
                                tree_node *index_node = node->leftmost_child->sibling;
                                if(index_node != NULL){
                                    if(index_node->token.name != NUM){
                                        if(index_node->token.name != ID){
                                            print_error("SEMANTIC ERROR", "INVALID ARRAY ACCESS");
                                        }
                                        else{
                                            type *type_ptr = (type*)search_hash_table_ptr_val(curr_sym_tab.table, index_node->token.str);
                                            if(type_ptr){
                                                type index_id_type = *type_ptr;
                                                if(index_id_type.name != INTEGER){
                                                    print_error("SEMANTIC ERROR", "ARRAY INDEX NOT INT");
                                                }
                                            }                                    
                                        }
                                    }else{
                                        int index = index_node->token.num;
                                        int lb = type_ptr->typeinfo.array.range_low;
                                        int ub = type_ptr->typeinfo.array.range_high;
                                        if(! (index >= lb && index <= ub) )
                                            print_error("SEMANTIC ERROR", "INDEX OUT OF BOUNDS");                                       
                                    }
                                }
                            }
                        }
                    
                        printf("LHS_TYPE : %s, RHS_TYPE : %s\n", terminal_string[lhs_type.name], terminal_string[rhs_type.name]);
                        
                        if(lhs_type.name != rhs_type.name){
                            print_error("SEMANTIC ERROR", "lhs and rhs types don't match");
                        }
                        
                        else{
                            printf("=======Type matches=======\n");
                        }
                    }
                }   
                /**
                 * @brief Type checking ends here
                 * 
                 */

            }   // if non-terminal
            else{   // terminal
                if(key_present_in_table(curr_sym_tab.table, node->token.str)){
                    type *type_ptr = (type*)search_hash_table_ptr_val(curr_sym_tab.table, node->token.str);
                    if(type_ptr){
                        printf("Received entry for %s, type_name = %s\n", node->token.str, terminal_string[type_ptr->name] );
                        if(type_ptr->name == ARRAY){
                            if(type_ptr->typeinfo.array.is_dynamic){
                                printf(" Dynamically indexed ");
                            }
                            printf("Range: [%d..%d] Primitive type : %s\n", type_ptr->typeinfo.array.range_low, type_ptr->typeinfo.array.range_high, terminal_string[type_ptr->typeinfo.array.primitive_type]);
                        }
                        else if(type_ptr->name == MODULE){
                            printf("is_declared : %d\n", type_ptr->typeinfo.module.is_declared);
                            printf("Input_types_list : ");
                            print_types_list(type_ptr->typeinfo.module.input_types);
                            printf("\n");
                            printf("Output_types_list : ");
                            print_types_list(type_ptr->typeinfo.module.output_types);
                            printf("\n");

                        }
                    }
                }
            }

            if (node->sibling != NULL) {
                node = node->sibling;
                // printf("node = node->sibling\n");
                // print_symbol_(node);
            } else {
                node = node->parent;
                // printf("node = node->parent\n");
                // print_symbol_(node);
            }
        } 
    }while (node != NULL);;
}