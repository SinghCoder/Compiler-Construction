/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef LEXERDEF_H
#define LEXERDEF_H

#include "hashtable.h"
#include "driver.h"

#define BUFFER_SIZE 1000

typedef enum {
#include "tokens.txt"
} token_name;

typedef struct token {
  token_name name;
  union {
    char str[MAX_LEXEME_LEN];
    int num;
    double rnum;
  };
  int line_no;
} TOKEN;

int state;
int just_retracted;

hash_element lookup_table[HASH_SIZE];

char buffer[BUFFER_SIZE];
char lexeme[MAX_LEXEME_LEN];

int lexeme_begin;
int forward_ptr;
int num_of_rounds;
int line_no;

extern char terminal_string[NUM_OF_TERMINALS][MAX_SYMBOL_LENGTH];

#endif