#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fast Modulo Exponentation
int fastModExp(int a, int b, int m) {
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
    hash_value += ((str[i] * fastModExp(PRIME, i, HASH_SIZE)) % HASH_SIZE);
  }

  return hash_value % HASH_SIZE;
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

// initialize keywordTable with keywords
hash_table init_hash_table() {

  hash_table table = (hash_table)malloc(sizeof(hash_element) * HASH_SIZE);
  for (int i = 0; i < HASH_SIZE; i++) {
    table[i].present = false;
  }
  return table;
}

int searchHashTable(hash_table table, char *lexeme) {
  int hashValue = hash(lexeme);
  int probe_num = 1;
  printf("comparing %s\n", table[hashValue].lexeme);
  fflush(stdout);
  while (table[hashValue].present == true) {
    if (strcmp(table[hashValue].lexeme, lexeme) == 0) {
      return table[hashValue].value;
    }
    hashValue = (hashValue + probe_num * probe_num) % HASH_SIZE;
    probe_num++;
  }

  return -1;
}
