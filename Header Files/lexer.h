#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

enum token_name{
    DEF, MODULE, ENDDEF,
    DRIVERDEF, DRIVERENDDEF,
    TAKES, INPUT, 
    SQBO, SQBC,
    RETURNS,
    START, END,
    DECLARE, ID, COLON, 
    ARRAY, OF, INTEGER, REAL, BOOLEAN, 
    TRUE, FALSE,
    ASSIGNOP, NUM, RNUM, SEMICOL,
    DRIVER, PROGRAM,
    GET_VALUE, PRINT,
    USE, WITH, PARAMETERS, COMMA,
    FOR, IN, RANGEOP, WHILE,
    SWITCH, BO, BC, CASE, BREAK, DEFAULT,
    PLUS, MINUS,
    MUL, DIV,
    LT, LE, GT, GE, EQ, NE,
    AND, OR
}

typedef enum { false, true } bool;  // boolean type

char look;      // lookahaead character

FILE *getStream(FILE *fp);

TOKEN getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);

/*
    report error
*/
void lexError(char *errStr);  

/*
    Initialization (if any)
*/
void init();
#endif
