#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "treeADTDef.h"
#include "hashtable.h"
#include "semantic_analyzerDef.h"

void construct_symtable(tree_node *ast_root);
void semantic_analyzer_init();
void insert_in_sym_table(tree_node *node);
void insert_function_definition(char *lexeme, tree_node *inp_par_node_list, tree_node *outp_par_node_list);

void insert_type_in_list(types_list *list, type *t);
// struct symbol_table_wrapper curr_sym_tab;
st_wrapper curr_sym_tab;

#endif