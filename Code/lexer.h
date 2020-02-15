#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "lexerDef.h"

void getStream(FILE *fp);

TOKEN getNextToken(FILE *fp);

char getChar(FILE *fp); 

void removeComments(char *testcaseFile, char *cleanFile);

void lexError(char *errStr, FILE* fp);

TOKEN getToken();

void retract(int num);

token_name searchLookupTable(char* lexeme);

void init();

#endif
