#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

void grammar_fill(FILE *fp);

symbol get_symbol_val(char str[]);

void insert_at_end(rhsnode_ptr *tail, symbol sym, type_of_sym tag);

void print_grammar();

#endif