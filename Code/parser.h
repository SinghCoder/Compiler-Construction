#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

grammar_t get_rules(FILE *fp);

symbol get_symbol_val(char str[]);

rhsnode_ptr insert(rhsnode_ptr, symbol sym, type_of_sym tag);

void print_grammar(grammar_t g);

#endif