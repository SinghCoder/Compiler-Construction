/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/

#ifndef HASHTABLEDEF_H
#define HASHTABLEDEF_H

#define PRIME 119
#define HASH_SIZE 67
#define MAX_LEXEME_LEN 100
#define KEY_NOT_FOUND -1

typedef struct {
		char lexeme[MAX_LEXEME_LEN];
		void *value;
		bool present;
} hash_element;

typedef hash_element hash_table[HASH_SIZE];

typedef struct symbol_table_wrapper{
	hash_table table;
	int level_num;
	struct symbol_table_wrapper *parent_table;
	struct symbol_table_wrapper *leftmost_child_table;
	struct symbol_table_wrapper *rightmost_child_table;
	struct symbol_table_wrapper *sibling_table;
}st_wrapper;

#endif
