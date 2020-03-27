#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "treeADTDef.h"
#include "hashtable.h"
#include "semantic_analyzerDef.h"

typedef enum { ARITH_OP, REL_OP, LOGICAL_OP, NO_MATCHING_OP} operator;

void construct_symtable(tree_node *ast_root);
void semantic_analyzer_init();
void insert_in_sym_table(tree_node *node);
void insert_function_definition(char *lexeme, tree_node *inp_par_node_list, tree_node *outp_par_node_list);

void insert_type_in_list(types_list *list, type *t);
type get_expr_type(tree_node *expr_node);

struct symbol_table_wrapper curr_sym_tab;

operator get_operator(tree_node *node);

type get_EopE_type(type t1, operator op, type t2);

void print_symbol_(tree_node *temp);

#endif