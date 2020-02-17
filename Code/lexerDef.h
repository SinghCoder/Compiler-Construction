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
    AND, OR,
    END_OF_FILE, LEX_ERROR
} token_name;

typedef enum { false, true } bool;  // boolean type

typedef struct token {
    char name[MAX_LEXEME_LENGTH];
    int line_no;
    union value{
        char lexeme[MAX_LEXEME_LENGTH];
        int num;
        float rnum;
    };
} TOKEN;

char look;      // lookahead character

int state;
int lex_len;
int id_count;
int line_count;
int f_ptr;
int lexeme_begin;
int c; //la char

char buffer[BUFFER_SIZE];
char lexeme[MAX_LEXEME_LENGTH];
char *err_msg;
char la_str[2];
char **terminal_string = {
    "DEF", "MODULE", "ENDDEF",
    "DRIVERDEF", "DRIVERENDDEF",
    "TAKES", "INPUT", 
    "SQBO", "SQBC",
    "RETURNS",
    "START", "END",
    "DECLARE", "ID", "COLON",
    "ARRAY", "OF", "INTEGER", "REAL", "BOOLEAN", 
    "TRUE", "FALSE",
    "ASSIGNOP", "NUM", "RNUM", "SEMICOL",
    "DRIVER", "PROGRAM",
    "GET_VALUE", "PRINT",
    "USE", "WITH", "PARAMETERS", "COMMA",
    "FOR", "IN", "RANGEOP", "WHILE",
    "SWITCH", "BO", "BC", "CASE", "BREAK", "DEFAULT",
    "PLUS", "MINUS",
    "MUL", "DIV",
    "LT", "LE", "GT", "GE", "EQ", "NE",
    "AND", "OR",
    "END_OF_FILE", "LEX_ERROR"
}
#endif