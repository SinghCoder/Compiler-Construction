/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef INTERMED_CODE_GEN_H
#define INTERMED_CODE_GEN_H
#include "intermed_codegen_def.h"
#include "semantic_analyzer.h"
#include "treeADT.h"
void generate_ir(tree_node *ast_node);
void intermed_codegen_init();
void code_emit(tac_op op, char *arg1_str, char *arg2_str, char *result_str, st_wrapper *curr_scope_table_ptr, type *encl_fun_type_ptr, char *cnstrct_beg_lbl);
void print_quadruples();
char *newlabel();
tree_node *newtemp(tree_node *expr1_node, operator op, tree_node *expr2_node, token_name type_name);
char *node2_tkn_str_val(tree_node *node);
tac_op get_tac_op(tree_node *node);
#endif