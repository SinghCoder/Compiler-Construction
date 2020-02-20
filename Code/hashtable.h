#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "lexerDef.h"

#define PRIME 119
#define HASH_SIZE 37

typedef struct keyword_element {
    char lexeme[BUFFER_SIZE];
    token_name symbol;
    bool present;
} keyword_element;

keyword_element table[HASH_SIZE];

int fastModExp(int a, int b, int m);

void init_hash_table();

int hash(char *str);

void insert(char *lexeme, token_name symbol);

token_name searchLookupTable(char *lexeme);

#endif