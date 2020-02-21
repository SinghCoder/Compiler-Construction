#ifndef PARSERDEF_H
#define PARSERDEF_H
#include "lexerDef.h"

#define MAX_LENGTH 100
#define NUM_OF_RULES 99
#define NUM_OF_NONTERMINALS 53
#define BITSTRING_PART_NUM 3
#define sz(x) sizeof(x) 
typedef unsigned long long int ull;
#define NUM_BITS (8 * sz(ull))

typedef enum
{
    MAINPROGRAM,
    MODULEDECLARATIONS,
    MODULEDECLARATION,
    OTHERMODULES,
    DRIVERMODULE,
    NTMODULE,
    MODULEDEF,
    RET,
    INPUT_PLIST,
    INPUT_PLIST_DASH,
    OUTPUT_PLIST,
    OUTPUT_PLIST_DASH,
    DATATYPE,
    TYPE,
    STATEMENTS,
    STATEMENT,
    IOSTMT,
    LITERAL,
    VAR,
    WHICHID,
    SIMPLESTMT,
    ASSIGNMENTSTMT,
    WHICHSTMT,
    LVALUEIDSTMT,
    LVALUEARRSTMT,
    INDEX,
    MODULEREUSESTMT,
    OPTIONAL,
    IDLIST,
    IDLIST_DASH,
    EXPRESSION,
    RESTEXPRESSION,
    RELATIONALEXPR,
    RESTRELATIONALEXPR,
    LOGICALOP,
    RELATIONALOP,
    UNARYARITHMETICEXPR,
    RESTUNARYARITHMETICEXPR,
    NONUNARYARITHMETICEXPR,
    ARITHMETICEXPRDASH,
    TERM,
    TERM_DASH,
    FACTOR,
    ADDSUBOP,
    MULDIVOP,
    DECLARESTMT,
    CONDITIONALSTMT,
    CASESTMT,
    OTHERCASE,
    VALUE,
    DEFAULTSTMT,
    ITERATIVESTMT,
    RANGE
} nonterminal;

char non_terminal_string [NUM_OF_NONTERMINALS][25];

typedef enum {T, NT} type_of_sym;

typedef union
{
    token_name t;
    nonterminal nt;
} symbol;

struct rhsnode
{
    symbol s;
    type_of_sym flag;
    struct rhsnode *next;
};

typedef struct rhsnode rhsnode;
typedef struct rhsnode *rhsnode_ptr;

typedef struct 
{
    nonterminal sym;
    rhsnode_ptr head;
    rhsnode_ptr tail;
} cell;

unsigned long long int first_set[NUM_OF_NONTERMINALS][BITSTRING_PART_NUM];   //f_set for each nt calculated by considering bits of 3 unsigned long long int
// typedef cell *grammar_t;
cell grammar[NUM_OF_RULES];
#endif
