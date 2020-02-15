#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

FILE *get_stream(FILE *fp);

TOKEN get_next_token();

void remove_comments(char *testcaseFile, char *cleanFile);

void lex_error(char *errStr);  

void init();

#endif
