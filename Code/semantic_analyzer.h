#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "treeADTDef.h"
#include "semantic_analyzerDef.h"
#include "hashtable.h"

typedef enum { ARITH_OP, REL_OP, LOGICAL_OP, NO_MATCHING_OP} operator;

void construct_symtable(tree_node *ast_root);
void semantic_analyzer_init();
st_wrapper *symbol_table_init();
void print_a_type(type *type_ptr);
type *create_type();
void *key_search_recursive(st_wrapper *sym_table,char *lexeme, type *encl_fun_type_ptr, bool *is_outp_param);
void insert_in_sym_table(st_wrapper *sym_table,tree_node *node);
void insert_function_definition(st_wrapper *sym_table,char *lexeme, tree_node *inp_par_node_list, tree_node *outp_par_node_list, int def_line_num);
void print_symbol_table(st_wrapper *sym_table);
void insert_param_in_list(params_list *list, type *t, char *param_name);
type get_expr_type(tree_node *expr_node, st_wrapper *sym_tab_ptr);

void print_params_list(params_list *list);

operator get_operator(tree_node *node);

type get_EopE_type(type t1, operator op, type t2);

void print_symbol_(tree_node *temp);

void verify_assignment_semantics(tree_node *assign_node, st_wrapper *curr_sym_tab_ptr);
void verify_switch_semantics(tree_node *switch_node, st_wrapper *curr_sym_tab_ptr);
void verify_fncall_semantics(tree_node *node, st_wrapper *curr_sym_tab_ptr);
void verify_fndefn_semantics(tree_node *node, st_wrapper *curr_sym_tab_ptr);
void verify_construct_semantics(tree_node *node);
void verify_declarations_validity(tree_node *node);
void verify_whileloop_semantics(tree_node *while_node, st_wrapper *curr_sym_tab_ptr);
type* check_encl_fun_params(type *fun_type, char *lexeme, bool *is_outp_param);
void mark_outp_param_assigned(char *id_str, type *fn_type_ptr);
void compare_args_list(params_type parm_type, tree_node *fncall_args_list, params_list *fndefn_params_list, type *encl_fun_type_ptr, int line_no, st_wrapper *curr_sym_tab_ptr);
bool is_types_matching(type *t1, type *t2);
void mark_while_loop_var_assigned(tree_node *while_node, char *var_name);
bool is_id_part_of_while_loop(tree_node *id_node/*In param*/, tree_node **while_node, int *child_dir /*Out params*/);
void install_id_in_loop_args(tree_node *while_node, char *id_str);
void second_ast_pass(tree_node *ast_root);
void print_arrays(st_wrapper *sym_tab_ptr);
void print_activation_records();
#endif