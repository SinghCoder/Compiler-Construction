#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"
#include <stdio.h>

void getStream(FILE *fp);

TOKEN getNextToken(FILE *fp);

char getChar(FILE *fp);

void lexError(char *errStr, FILE *fp);

TOKEN getToken();

void retract(int num);

void lexer_init();

token_name searchLookupTable(char *lexeme);

void print_token_stream(FILE *source);

void lookup_table_fill();

void reset_lexer_dfa(FILE *source);

#endif
