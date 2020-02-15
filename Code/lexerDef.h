#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdio.h>

#define BUFFER_SIZE 20

typedef enum {
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
    AND, OR,
    LEX_ERROR,      //VERIFY
    END_OF_FILE,    //VERIFY
    DELIM   //VERIFY
} token_name;

typedef struct token {
    token_name name;
    union{
        char *str;
        int num;
        double rnum;
    };
} TOKEN;

int state;
int just_retracted;

char buffer[BUFFER_SIZE];
char lexeme[BUFFER_SIZE];

int lexeme_begin;
int forward_ptr;
int line_no;

#endif