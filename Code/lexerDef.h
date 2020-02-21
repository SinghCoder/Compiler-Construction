#ifndef LEXERDEF_H
#define LEXERDEF_H

#include "hashtable.h"

#define BUFFER_SIZE 20
#define NUM_OF_TERMINALS 60

typedef enum {
#include "tokens.txt"
} token_name;

typedef struct token {
  token_name name;
  union {
    char *str;
    int num;
    double rnum;
  };
  int line_no;
} TOKEN;

// typedef enum { false, true } bool;  // boolean type

int state;
int just_retracted;

hash_table lookup_table;

char buffer[BUFFER_SIZE];
char lexeme[BUFFER_SIZE];

int lexeme_begin;
int forward_ptr;
int line_no;
char terminal_string[NUM_OF_TERMINALS][50];

#endif