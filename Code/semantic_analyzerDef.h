#ifndef SEMANTIC_ANALYZER_DEF_H
#define SEMANTIC_ANALYZER_DEF_H

#include "driver.h"
#include "lexerDef.h"
#include <limits.h>

#define OBTAIN_DYNAMICALLY 0x3f3f3f3f
#define WIDTH_BOOLEAN 8
#define WIDTH_INTEGER 8
#define WIDTH_REAL 8	
#define WIDTH_POINTER 8
#define MAX_VARSNUM_IN_EXPR 100
#define WHILE_LHS 0
#define WHILE_RHS 1
#define DONT_CARE -1

extern char non_terminal_string[NUM_OF_NONTERMINALS][MAX_SYMBOL_LENGTH];
extern char terminal_string[NUM_OF_TERMINALS][MAX_SYMBOL_LENGTH];
typedef struct params_list_node params_list_node;
typedef struct type type;

typedef enum {input, output} params_type;

struct params_list_node{
    type *t;
    char param_name[MAX_LEXEME_LEN];
    params_list_node *next;
};

typedef struct params_list{
    params_list_node *first;
    params_list_node *last;    
    int length;
} params_list;

struct type{
    token_name name;
    union{
        struct{
            token_name primitive_type;
            union{
                int value;
                char lexeme[MAX_LEXEME_LEN];
            } range_low;
            union{
                int value;
                char lexeme[MAX_LEXEME_LEN];
            } range_high;            
            struct{
                bool range_low;
                bool range_high;
            }is_dynamic;
        }array;

        struct{
            params_list *input_params;
            params_list *output_params;
            char module_name[MAX_LEXEME_LEN];
            bool is_declared;
            bool is_defined;
            bool is_declrn_valid;
            int curr_offset;
            int base_addr;
        }module;
            
    } typeinfo;
    bool is_assigned;
    int width;
    int offset;
};

st_wrapper *curr_sym_tab_ptr;
st_wrapper *root_sym_tab_ptr;

#endif
