#ifndef CODE_GEN_DEF_H
#define CODE_GEN_DEF_H
#include "driver.h"
#include "treeADTDef.h"

int label_count;
int temp_count;
int quad_count;

char tac_op_str[NUM_TAC_OP][MAX_SYMBOL_LENGTH];

typedef enum{
    PLUS_OP,
    MINUS_OP,
    MUL_OP,
    DIV_OP,
    GE_OP,
    GT_OP,
    LE_OP,
    LT_OP,
    EQ_OP,
    NE_OP,
    AND_OP,
    OR_OP,
    UMINUS_OP,
    LABEL_OP,
    INPUT_OP,
    OUTPUT_OP,
    ASSIGN_OP,
    GOTO_UNCOND_OP,
    IF_TRUE_GOTO_OP,
    IF_FALSE_GOTO_OP,
    PARAM_OP,
    PROC_CALL_OP,
    INDEXED_COPY_OP,
    ARRAY_ACCESS_OP,
    SWITCH_OP,
    GOTO_IF_MATCHES,
    DEFAULT_OP
}tac_op;

typedef struct{
    tac_op op;
    char *arg1;
    char *arg2;
    char *result;
    tree_node *node_arg1;
    tree_node *node_arg2;
    tree_node *node_res;
}quad_node;

quad_node quadruples[MAX_NUM_QUADRUPLES];

#endif