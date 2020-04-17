/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef AST_H
#define AST_H

#include "treeADTDef.h"

bool ends_in_dash(char *label);
bool is_base_case(tree_node *node);
bool is_recursion(tree_node *node);
bool is_important(tree_node *node);
tree_node *copy_node(tree_node *src);
void extend_inh_node(tree_node *node1, tree_node *node2);
tree_node *construct_ast(tree_node *parse_tree_root);
void traverse_ast(tree_node *ast_node);
void populate_start_end_mapping(tree_node *parse_tree_root);

#endif