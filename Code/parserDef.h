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
#define NUM_OF_AST_LABELS 22
#define NO_MATCHING_RULE -1
#define COLUMN_SIZE 20
#define MAX_FILE_NAME_LEN 100

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum
{
    #include "non_terminals.txt"
} nonterminal;

typedef enum
{
    #include "ast_node_labels.txt"
} ast_label;

char non_terminal_string [NUM_OF_NONTERMINALS][50];
char ast_label_string [NUM_OF_AST_LABELS][50];

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

char parse_tree_file[MAX_FILE_NAME_LEN];
FILE *parse_tree_file_ptr;

#endif
