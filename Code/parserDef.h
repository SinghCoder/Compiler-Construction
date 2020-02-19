#ifndef PARSER_H
#define PARSER_H
#include "lexerDef.h"
#define NUM_OF_RULES 100

typedef enum
{
    "PROGRAM",
    "MODULEDECLARATIONS",
    "MODULEDECLARATION",
    "OTHERMODULES",
    "DRIVERMODULE",
    "MODULE",
    "MODULEDEF",
    "RET",
    "INPUT_PLIST",
    "INPUT_PLIST_DASH",
    "OUTPUT_PLIST",
    "OUTPUT_PLIST_DASH",
    "DATATYPE",
    "TYPE",
    "STATEMENTS",
    "STATEMENT",
    "IOSTMT",
    "LITERAL",
    "VAR",
    "WHICHID",
    "SIMPLESTMT",
    "ASSIGNMENTSTMT",
    "WHICHSTMT",
    "LVALUEIDSTMT",
    "LVALUEARRSTMT",
    "INDEX",
    "MODULEREUSESTMT",
    "OPTIONAL",
    "IDLIST",
    "IDLIST_DASH",
    "EXPRESSION",
    "RESTEXPRESSION",
    "RELATIONALEXPR",
    "RESTRELATIONALEXPR",
    "LOGICALOP",
    "RELATIONALOP",
    "UNARYARITHMETICEXPR",
    "RESTUNARYARITHMETICEXPR",
    "NONUNARYARITHMETICEXPR",
    "ARITHMETICEXPRDASH",
    "TERM",
    "TERM_DASH",
    "FACTOR",
    "ADDSUBOP",
    "MULDIVOP",
    "DECLARESTMT",
    "CONDITIONALSTMT",
    "CASESTMT",
    "OTHERCASE",
    "VALUE",
    "DEFAULT",
    "ITERATIVESTMT",
    "RANGE"
} nonterminal;

typedef enum {T, NT} type_of_sym;

typedef union
{
    terminal t;
    nonterminal nt;
} symbol;

struct rhsnode
{
    symbol s;
    type_of_sym flag;
    struct rhsnode *next;
};

typedef struct rhsnode RHSNODE;

typedef struct cell
{
    nonterminal sym;
    RHSNODE *head;
} GRAMMAR[ NUM_OF_RULES ];

#endif