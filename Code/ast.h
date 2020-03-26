#ifndef AST_H
#define AST_H

#include "treeADTDef.h"


void ast_init(char non_terminal_string[NUM_OF_NONTERMINALS][MAX_SYMBOL_LENGTH]);
bool ends_in_dash(char *label);
bool is_base_case(tree_node *node);
bool is_recursion(tree_node *node);
bool is_important(tree_node *node);
tree_node *copy_node(tree_node *src);
void extend_inh_node(tree_node *node1, tree_node *node2);
tree_node *construct_ast(tree_node *parse_tree_root);
void print_ast(tree_node *root);
void print_symbol(tree_node *node);

#endif