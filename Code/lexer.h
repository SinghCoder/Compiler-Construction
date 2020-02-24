/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"
#include <stdio.h>

void populate_buffer(FILE *fp);

void populate_lookup_table();

void populate_terminal_string();

TOKEN get_next_token(FILE *fp);

char get_char(FILE *fp);

TOKEN get_token();

void retract(int num);

void lexer_init(FILE *source);

token_name search_lookup_table(char *lexeme);

void tokenize_source_file(FILE *source);

void remove_comments(FILE *source, char *no_comment_file);

#endif
