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

// Fast Modulo Exponentation
int fast_mod_exp(int a, int b, int m) {
  int res = 1;
  while (b > 0) {
    if (b & 1) {
      res = (res * a) % m;
    }
    a = (a * a) % m;
    b >>= 1;
  }
  return res;
}

// Takes a string and generates corresponding hash value
int hash(char *str) {
  int n = strlen(str);
  int hash_value = 0;

  for (int i = 0; i < n; i++) {
    hash_value += ((str[i] * fast_mod_exp(PRIME, i, HASH_SIZE)) % HASH_SIZE);
  }

  return hash_value % HASH_SIZE;
}

// initialize keywordTable with keywords
void init_hash_table(hash_table table) {
  // hash_table table = (hash_table)malloc(sizeof(hash_element) * HASH_SIZE);
  if (table == NULL) {
    perror("HASH table allocation error: memory not available\n");
    // return;
    exit(1);
  }
  for (int i = 0; i < HASH_SIZE; i++) {
    table[i].present = false;
    table[i].value = 0; // junk initialization
  }
  // return table;
}

// recursively called to add keyword to keywordTable
void hash_insert(hash_table table, char *lexeme, int value) {
  int hashValue;
  hashValue = hash(lexeme);
  // printf("hash obtained\n");
  int probe_num = 1;
  while (table[hashValue].present == true) {
    hashValue = (hashValue + probe_num * probe_num) % HASH_SIZE;
    probe_num++;
  }
  strcpy(table[hashValue].lexeme, lexeme);

  table[hashValue].present = true;
  table[hashValue].value = value;
}

int search_hash_table(hash_table table, char *lexeme) {
  int hashValue = hash(lexeme);
  int probe_num = 1;
  // printf("comparing %s\n", table[hashValue].lexeme);
  // fflush(stdout);
  while (table[hashValue].present == true) {
    if (strcmp(table[hashValue].lexeme, lexeme) == 0) {
      return table[hashValue].value;
    }
    hashValue = (hashValue + probe_num * probe_num) % HASH_SIZE;
    probe_num++;
  }
  return KEY_NOT_FOUND;
}
