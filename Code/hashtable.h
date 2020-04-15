/***************************************
				|GROUP-09|
	Aditya Upadhyay      -   2017A7PS0083P
	Harpider Jot Singh   -   2017A7PS0057P
	Jaladi Lakshmi Teja  -   2017A7PS0068P
	Vishal Mittal        -   2017A7PS0080P
	Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include "hashtableDef.h"

int fast_mod_exp(int a, int b, int m);

void init_hash_table(hash_table);

int hash(char *str);

void hash_insert(hash_table table, char *lexeme, int value);

int search_hash_table(hash_table table, char *lexeme);

void hash_insert_ptr_val(hash_table table, char *lexeme, void *value_ptr);

void* search_hash_table_ptr_val(hash_table table, char *lexeme);

bool key_present_in_table(hash_table table, char *lexeme);

#endif