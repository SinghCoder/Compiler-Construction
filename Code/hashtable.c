/***************************************
								|GROUP-09|
	Aditya Upadhyay      -   2017A7PS0083P
	Harpider Jot Singh   -   2017A7PS0057P
	Jaladi Lakshmi Teja  -   2017A7PS0068P
	Vishal Mittal        -   2017A7PS0080P
	Yash Vijay           -   2017A7PS0072P
*****************************************/
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Calculates (a^b)%m in O(logb) time
 * 
 * @param a base
 * @param b exponent
 * @param m modulus
 * @return (a^b) % m
 */

int fast_mod_exp(int a, int b, int m) {
	int res = 1;
	while (b > 0) 
	{
		if (b & 1) 
		{
			res = (res * a) % m;
		}
		a = (a * a) % m;
		b >>= 1;
	}
	return res;
}

/**
 * @brief Takes a string and generates corresponding hash value
 * 
 * @param str string
 * @return hash value
 */
int hash(char *str) {
	int n = strlen(str);
	int hash_value = 0;

	for (int i = 0; i < n; i++) {
		hash_value += ((str[i] * fast_mod_exp(PRIME, i, HASH_SIZE)) % HASH_SIZE);
	}

	return hash_value % HASH_SIZE;
}

/**
 * @brief initialize a hash table
 * 
 */
void init_hash_table(hash_table table) {
	
	if (table == NULL) 
	{
		perror("HASH table allocation error: memory not available\n");
		exit(1);
	}
	for (int i = 0; i < HASH_SIZE; i++) 
	{
		table[i].present = false;
		table[i].value = malloc(sizeof(int));
		*(int*)(table[i].value) = 0; 
	}
}

/**
 * @brief recursively called to add keyword to keywordTable using quadratic probing
 * 
 * @param table table
 * @param lexeme lexeme to be added to table
 * @param value hash value for the lexeme
 */
void hash_insert(hash_table table, char *lexeme, int value) {
	int hashValue;
	hashValue = hash(lexeme);
	int probe_num = 1;
	while (table[hashValue].present == true) 
	{
			hashValue = (hashValue + probe_num * probe_num) % HASH_SIZE;
			probe_num++;
	}
	strcpy(table[hashValue].lexeme, lexeme);

	table[hashValue].present = true;
	table[hashValue].value = malloc(sizeof(int));
	*(int*)(table[hashValue].value) = value;
}

void hash_insert_ptr_val(hash_table table, char *lexeme, void *value_ptr){
	printf("Inserting in hash table %s\n", lexeme);
	int hashValue;
	hashValue = hash(lexeme);
	int probe_num = 1;
	while (table[hashValue].present == true) 
	{
			hashValue = (hashValue + probe_num * probe_num) % HASH_SIZE;
			probe_num++;
	}
	strcpy(table[hashValue].lexeme, lexeme);

	table[hashValue].present = true;
	table[hashValue].value = value_ptr;

}

/**
 * @brief search a string in the hash table
 * 
 * @return int : hash value else -1 if lexeme not found
 */

int search_hash_table(hash_table table, char *lexeme) {
	int hashValue = hash(lexeme);
	int probe_num = 1;
	while (table[hashValue].present == true) 
	{
		if (strcmp(table[hashValue].lexeme, lexeme) == 0) 
		{
			return *(int*)(table[hashValue].value);
		}
		hashValue = (hashValue + probe_num * probe_num) % HASH_SIZE;
		probe_num++;
	}
	return KEY_NOT_FOUND;
}

bool key_present_in_table(hash_table table, char *lexeme){
	int hashValue = hash(lexeme);
	int probe_num = 1;
	while (table[hashValue].present == true) 
	{
		if (strcmp(table[hashValue].lexeme, lexeme) == 0) 
		{
			return true;
		}
		hashValue = (hashValue + probe_num * probe_num) % HASH_SIZE;
		probe_num++;
	}
	return false;
}

void* search_hash_table_ptr_val(hash_table table, char *lexeme){
	int hashValue = hash(lexeme);
	int probe_num = 1;
	while (table[hashValue].present == true) 
	{
		if (strcmp(table[hashValue].lexeme, lexeme) == 0) 
		{
			return table[hashValue].value;
		}
		hashValue = (hashValue + probe_num * probe_num) % HASH_SIZE;
		probe_num++;
	}
}