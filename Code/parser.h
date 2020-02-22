#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

void grammar_fill(FILE *fp);

symbol get_symbol_val(char str[]);

void insert_at_end(rhsnode_ptr *tail, symbol sym, type_of_sym tag);

void print_grammar();

void parser_init();

void populate_first_sets();

void populate_follow_sets();

unsigned long long int get_nullable_set();

void print_first_sets();

void print_follow_sets();

void print_first(nonterminal a);

void print_rule_fset(ull *fset);

ull* firstOf(nonterminal nt);

ull *get_rule_first_set(rhsnode_ptr node);

bool is_superset(ull a[BITSTRING_PART_NUM], ull b[BITSTRING_PART_NUM]);
#endif