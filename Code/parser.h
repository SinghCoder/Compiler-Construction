/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef PARSER_H
#define PARSER_H

// #include "parserDef.h"
#include "treeADT.h"
#include <stdio.h>

void grammar_fill(FILE *fp);

symbol get_symbol(char str[]);

void insert_at_end(rhsnode_ptr *tail, symbol sym);

void print_grammar();

void parser_init();

int rightmost_set_bit(unsigned long long *num);

void create_parse_table();

void populate_first_sets();

void populate_follow_sets();

void print_first_sets();

void print_follow_sets();

void print_first(nonterminal a);

void print_rule_fset(ull *fset);

void print_parse_table();

void print_parse_tree(tree_node *root);

void print_parse_tree_for_tool(tree_node *root, tree_type typ);

void print_rule(int rule_no);

tree_node *parse_input_source_code(FILE *source);

ull *get_rule_first_set(rhsnode_ptr node);

bool is_superset(ull a[SET_SIZE], ull b[SET_SIZE]);

void free_grammar();

void print_symbol(symbol sym);

void pretty_print(char *s);
#endif