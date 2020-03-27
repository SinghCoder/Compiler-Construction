#ifndef SEMANTIC_ANALYZER_DEF_H
#define SEMANTIC_ANALYZER_DEF_H

#include "driver.h"
#include <limits.h>

#define OBTAIN_DYNAMICALLY INT_MAX

extern char non_terminal_string[NUM_OF_NONTERMINALS][MAX_SYMBOL_LENGTH];
extern char terminal_string[NUM_OF_TERMINALS][MAX_SYMBOL_LENGTH];

typedef struct type type;

struct type{
    token_name name;
    union{
        struct{
            token_name primitive_type;
            int range_low;
            int range_high;
        }array;

        struct{
            type *input_types;
            type *output_types;
            bool is_declared;
        }module;
    } typeinfo;
};

#endif
