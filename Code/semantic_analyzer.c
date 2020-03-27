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

type *retreive_type(tree_node *node){
    type *type_ptr = (type*) malloc( sizeof(type) );
    if(node->sym.is_terminal == false)   //primitive type
    {
        type_ptr->name = ARRAY;
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
            }
            else{
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