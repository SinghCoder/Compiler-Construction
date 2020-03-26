#ifndef DRIVER_H
#define DRIVER_H

#include<stdio.h>

#define NUM_OF_TERMINALS 61
#define NUM_OF_NONTERMINALS 53
#define NUM_OF_RULES 98
#define MAX_SYMBOL_LENGTH 50
#define MAX_FILE_NAME_LEN 100
#define COLUMN_SIZE 20

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char terminal_string[NUM_OF_TERMINALS][MAX_SYMBOL_LENGTH];
char non_terminal_string[NUM_OF_NONTERMINALS][MAX_SYMBOL_LENGTH];

char parse_tree_file[MAX_FILE_NAME_LEN];
FILE *parse_tree_file_ptr;

#endif