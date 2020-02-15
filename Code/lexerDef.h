#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdio.h>

#define BUFFER_SIZE 512
#define TRAP_STATE -1
#define MAX_LEXEME_LENGTH 21
#define MAX_ERRMSG_LEN 50

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
    AND, OR
} token_name;

typedef enum { false, true } bool;  // boolean type

typedef struct token {
    token_name name;
    union value{
        char *str;
        int num;
        float rnum;
    };
} TOKEN;

char look;      // lookahead character

int state;
int lex_len;
int id_count;
int line_count;
int c;

char buffer1[BUFFER_SIZE];
char buffer2[BUFFER_SIZE];
char *err_msg;
char la_str[2];

#endif