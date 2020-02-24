/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef HASHTABLE_H
#define HASHTABLE_H

//#include "lexerDef.h"
#include<stdbool.h>

#define PRIME 119
#define HASH_SIZE 67
#define MAX_LEXEME_LEN 100
#define KEY_NOT_FOUND -1

typedef struct {
    char lexeme[MAX_LEXEME_LEN];
    int value;
    bool present;
} hash_element;

int fast_mod_exp(int a, int b, int m);

typedef hash_element *hash_table;       // change to static allocation

void init_hash_table(hash_table);

int hash(char *str);

void hash_insert(hash_table table, char *lexeme, int value);

int search_hash_table(hash_table table, char *lexeme);

#endif