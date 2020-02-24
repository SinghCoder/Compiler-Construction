/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"
#include<stdbool.h>
#include "setADT.h"

#define RHS_MAX_LENGTH 100
#define NUM_OF_RULES 99
#define NUM_OF_NONTERMINALS 53
#define NO_MATCHING_RULE -1
#define COLUMN_SIZE 20

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
    symbol sym;
    struct rhsnode *next;
} rhsnode;

typedef struct rhsnode *rhsnode_ptr;

typedef struct
{
    nonterminal lhs;
    rhsnode_ptr head;
    rhsnode_ptr tail;
} cell;

ull first_set[NUM_OF_NONTERMINALS][SET_SIZE];
ull follow_set[NUM_OF_NONTERMINALS][SET_SIZE];

cell grammar[NUM_OF_RULES];

hash_element terminal_table[HASH_SIZE];
hash_element non_terminal_table[HASH_SIZE];

bool error_present;

int parse_table[NUM_OF_NONTERMINALS][NUM_OF_TERMINALS];

#endif
