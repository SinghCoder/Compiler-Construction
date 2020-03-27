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
        
        type_ptr->typeinfo.array.range_low = ( node->leftmost_child->leftmost_child->sym.is_terminal) ? node->leftmost_child->leftmost_child->token.num : OBTAIN_DYNAMICALLY;
        type_ptr->typeinfo.array.range_high = ( node->leftmost_child->rightmost_child->sym.is_terminal) ? node->leftmost_child->rightmost_child->token.num : OBTAIN_DYNAMICALLY;
         
    }
    else{
        type_ptr->name = node->sym.t;
    }
    return type_ptr;
}

void insert_function_definition(char *lexeme, tree_node *inp_par_node_list, tree_node *outp_par_node_list){

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
        // print_symbol_(node);
        // printf("===================\n");
        if (node->visited == false) {
            node->visited = true;

            if(node->sym.is_terminal == false){
                if(is_new_scope_openable(node->sym.nt) == true && node->sym.nt != MAINPROGRAM){
                    st_wrapper *new_sym_tab_ptr = (st_wrapper*)malloc(sizeof(st_wrapper));
                    new_sym_tab_ptr->parent_table = &curr_sym_tab;
                    init_hash_table(new_sym_tab_ptr->table);
                    curr_sym_tab = *new_sym_tab_ptr;
                }
                else if(node->sym.nt == NTMODULE){
                    insert_function_definition(node->leftmost_child->token.str, node->leftmost_child->sibling, node->leftmost_child->sibling->sibling);
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
                }   
            }
            else{
                if(key_present_in_table(curr_sym_tab.table, node->token.str)){
                    type *type_ptr = (type*)search_hash_table_ptr_val(curr_sym_tab.table, node->token.str);
                    if(type_ptr){
                        printf("Received entry for %s, type_name = %s\n", node->token.str, terminal_string[type_ptr->name] );
                        if(type_ptr->name == ARRAY){
                            printf("Range: [%d..%d] Primitive type : %s", type_ptr->typeinfo.array.range_low, type_ptr->typeinfo.array.range_high, terminal_string[type_ptr->typeinfo.array.primitive_type]);
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