#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"
#include <stdio.h>

void grammar_fill(FILE *fp);

symbol get_symbol(char str[]);

void insert_at_end(rhsnode_ptr *tail, symbol sym);

void print_grammar();

void parser_init();

void print_first_sets();

void populate_first_sets();

int rightmost_set_bit(unsigned long long *num);

void createParseTable(ull **first, ull **follow);

#endif