/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef DRIVER_H
#define DRIVER_H

#include<stdio.h>

#define NUM_OF_TERMINALS 62
#define NUM_OF_NONTERMINALS 54
#define NUM_OF_RULES 99
#define MAX_SYMBOL_LENGTH 50
#define MAX_FILE_NAME_LEN 100
#define COLUMN_SIZE 20
#define NUM_TAC_OP 30
#define MAX_SWITCH_CASES 200

#define MAX_LABEL_LEN 20
#define MAX_TEMP_LEN 20
#define MAX_NUM_QUADRUPLES 500
#define MAX_ERROR_STRING_LENGTH 500
#define MAX_ERROR_NUM 500

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LEXICAL_ERROR 0
#define SYNTACTIC_ERROR 1
#define SEMANTIC_ERROR 2
#define INTERNAL_ERROR 3

char terminal_string[NUM_OF_TERMINALS][MAX_SYMBOL_LENGTH];
char non_terminal_string[NUM_OF_NONTERMINALS][MAX_SYMBOL_LENGTH];

typedef enum {ast, pt} tree_type;

char parse_tree_file[MAX_FILE_NAME_LEN];
char assembly_file[MAX_FILE_NAME_LEN];
FILE *parse_tree_file_ptr;
FILE *assembly_file_ptr;
int curr_start;
int curr_end;
int lex_err_num;
int syn_err_num;
int sem_err_num;
void store_error(int line_num, int type, char *message);

int num_tree_nodes;

char lexical_errors[MAX_ERROR_NUM][MAX_ERROR_STRING_LENGTH];
char syntactic_errors[MAX_ERROR_NUM][MAX_ERROR_STRING_LENGTH];
char semantic_errors[MAX_ERROR_NUM][MAX_ERROR_STRING_LENGTH];

void print_errors();

#endif