#ifndef INTERMED_CODE_GEN_DEF_H
#define INTERMED_CODE_GEN_DEF_H
#include "driver.h"
#include "treeADTDef.h"
int label_count;
int temp_count;
int quad_count;

char tac_op_str[NUM_TAC_OP][MAX_SYMBOL_LENGTH];
char *to_be_attached_label;
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
    UPLUS_OP,
    UMINUS_OP,
    LABEL_OP,
    INPUT_OP,
    OUTPUT_OP,
    ASSIGN_OP,
    GOTO_UNCOND_OP,
    IF_TRUE_GOTO_OP,
    INP_PARAM_OP,
    OUTP_PARAM_OP,
    PROC_CALL_OP,
    INDEXED_COPY_OP,
    ARRAY_ACCESS_OP,
    DECLARE_DYN_ARR_OP,
    DECLARE_STATIC_ARR_OP,
    RETURN_OP,
    FN_DEFN_OP,
    EXIT_PROGRAM_IF_TRUE_OP
}tac_op;

typedef struct{
    tac_op op;
    char *arg1;
    char *arg2;
    char *result;
    char *cnstrct_code_begin;
    st_wrapper *curr_scope_table_ptr;
    type *encl_fun_type_ptr;
}quad_node;

quad_node quadruples[MAX_NUM_QUADRUPLES];

char *switch_test_label;
typedef struct{
    char *label;
    char *value;
} switch_table_node;

switch_table_node switch_table[MAX_SWITCH_CASES];
int switch_tbl_entry_num;
#endif