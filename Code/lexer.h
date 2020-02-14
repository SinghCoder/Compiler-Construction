#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

FILE *getStream(FILE *fp);

TOKEN getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);

void lexError(char *errStr);  

void init();

#endif
