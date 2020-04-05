#ifndef CODE_GEN_H
#define CODE_GEN_H
#include "codegenDef.h"
#include "semantic_analyzer.h"
#include "treeADT.h"
void generate_ir(tree_node *ast_node);
void codegen_init();
void code_emit(tac_op op, char *arg1_str, char *arg2_str, char *result_str);
void print_quadruples();
char *newlabel();
tree_node *newtemp(tree_node *expr1_node, operator op, tree_node *expr2_node);
char *to_string(tree_node *node);
tac_op get_tac_op(tree_node *node);
#endif