#ifndef HASHTABLE_H
#define HASHTABLE_H

//#include "lexerDef.h"
#include<stdbool.h>

#define PRIME 119
#define HASH_SIZE 67

typedef struct {
    char lexeme[100];
    int value;
    bool present;
} hash_element;

int fastModExp(int a, int b, int m);

typedef hash_element *hash_table;

hash_table init_hash_table();

int hash(char *str);

void hash_insert(hash_table table, char *lexeme, int value);

int searchHashTable(hash_table table, char *lexeme);

#endif