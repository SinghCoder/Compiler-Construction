#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

enum TOKEN {
    INTEGER,
    REAL,
    BOOLEAN,
    ARRAY,
    TRUE,
    FALSE,
    OF,
    START,
    END,
    DECLARE,
    MODULE,
    DRIVER,
    PROGRAM,
    GET_VALUE,
    PRINT,
    USE,
    WITH,
    PARAMETERS,
    TAKES,
    INPUT,
    RETURNS,
    FOR,
    IN,
    SWITCH,
    CASE,
    BREAK,
    DEFAULT,
    WHILE,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LT,
    LE,
    GT,
    GE,
    EQ,
    NE,
    AND,
    OR,
    DEF,
    ENDDEF,
    DRIVERDEF,
    DRIVERENDDEF,
    COLON,
    RANGEOP,
    SEMICOL,
    COMMA,
    ASSIGNOP,
    SQBO,
    SQBC,
    BO,
    BC
}

FILE *getStream(FILE *fp);

TOKEN getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);

#endif