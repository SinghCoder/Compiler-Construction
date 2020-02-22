#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"
#include<stdbool.h>

#define RHS_MAX_LENGTH 100
#define NUM_OF_RULES 99
#define NUM_OF_NONTERMINALS 53
#define BITSTRING_PART_NUM 3

#define sz(x) sizeof(x) 
typedef unsigned long long int ull;
#define NUM_BITS (8 * sz(ull))

typedef enum
{
    #include "non_terminals.txt"
} nonterminal;

char non_terminal_string [NUM_OF_NONTERMINALS][50];

typedef struct{
    union
    {
        token_name t;
        nonterminal nt;
    };
    bool is_terminal;
} symbol;

typedef struct rhsnode
{
    symbol s;
    struct rhsnode *next;
} rhsnode;

typedef struct rhsnode *rhsnode_ptr;

typedef struct
{
    nonterminal lhs;
    rhsnode_ptr head;
    rhsnode_ptr tail;
} cell;

unsigned long long int first_set[NUM_OF_NONTERMINALS][BITSTRING_PART_NUM];   //f_set for each nt calculated by considering bits of 3 unsigned long long int
unsigned long long int follow_set[NUM_OF_NONTERMINALS][BITSTRING_PART_NUM];   //f_set for each nt calculated by considering bits of 3 unsigned long long int
// typedef cell *grammar_t;
cell grammar[NUM_OF_RULES];

hash_table terminal_table;
hash_table non_terminal_table;

int parse_table[NUM_OF_NONTERMINALS][NUM_OF_TERMINALS];

#endif
