#ifndef DRIVER_H
#define DRIVER_H

#include<stdio.h>

#define NUM_OF_TERMINALS 62
#define NUM_OF_NONTERMINALS 54
#define NUM_OF_RULES 99
#define MAX_SYMBOL_LENGTH 50
#define MAX_FILE_NAME_LEN 100
#define COLUMN_SIZE 20
#define NUM_TAC_OP 29
#define MAX_SWITCH_CASES 200

#define MAX_LABEL_LEN 20
#define MAX_TEMP_LEN 20
#define MAX_NUM_QUADRUPLES 500

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char terminal_string[NUM_OF_TERMINALS][MAX_SYMBOL_LENGTH];
char non_terminal_string[NUM_OF_NONTERMINALS][MAX_SYMBOL_LENGTH];

typedef enum {ast, pt} tree_type;

char parse_tree_file[MAX_FILE_NAME_LEN];
FILE *parse_tree_file_ptr;
FILE *ast_file_ptr;

void print_error(char *type, char *message);

int num_tree_nodes;

#endif