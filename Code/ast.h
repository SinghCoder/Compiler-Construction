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

#endif