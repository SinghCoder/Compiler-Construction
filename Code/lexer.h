#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"
#include <stdio.h>

void populateBuffer(FILE *fp);

void populate_lookup_table();

void populate_terminal_string();

TOKEN getNextToken(FILE *fp);

char getChar(FILE *fp);

TOKEN getToken();

void retract(int num);

void lexer_init(FILE *source);

token_name searchLookupTable(char *lexeme);

void tokenize_source_file(FILE *source);

void remove_comments(FILE *source, char *no_comment_file);

#endif
