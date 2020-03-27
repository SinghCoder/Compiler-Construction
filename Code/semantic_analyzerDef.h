#ifndef SEMANTIC_ANALYZER_DEF_H
#define SEMANTIC_ANALYZER_DEF_H

#include "driver.h"
#include <limits.h>

#define OBTAIN_DYNAMICALLY INT_MAX

extern char non_terminal_string[NUM_OF_NONTERMINALS][MAX_SYMBOL_LENGTH];
extern char terminal_string[NUM_OF_TERMINALS][MAX_SYMBOL_LENGTH];

typedef struct types_list_node types_list_node;
typedef struct type type;

struct types_list_node{
    type *t;
    types_list_node *next;
};

typedef struct types_list{
    types_list_node *first;
    types_list_node *last;    
} types_list;

struct type{
    token_name name;
    union{
        struct{
            token_name primitive_type;
            int range_low;
            int range_high;
            bool is_dynamic;
        }array;

        struct{
            types_list *input_types;
            types_list *output_types;
            bool is_declared;
        }module;
    } typeinfo;
};

#endif
