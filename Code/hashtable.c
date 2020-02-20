#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashtable.h"

//Fast Modulo Exponentation
int fastModExp(int a, int b, int m) {
    int res = 1;
    while(b > 0) {
        if(b & 1){
            res = (res*a) % m;
        }
        a = (a*a) % m;
        b >>= 1;
    }
    return res;
}

// Takes a string and generates corresponding hash value
int hash(char *str) {
    int n = strlen(str);
    int hash_value  = 0;

    for(int i = 0; i < n; i++) {
        hash_value += ((str[i] * fastModExp(PRIME, i, HASH_SIZE)) % HASH_SIZE);
    }

    return hash_value % HASH_SIZE;
}

//recursively called to add keyword to keywordTable
void insert(char *lexeme, token_name symbol) { 
	int hashValue;
	hashValue = hash(lexeme);
    // printf("hash obtained\n");
    int probe_num = 1;
	while (table[hashValue].present == true)
    {
		hashValue = (hashValue + probe_num*probe_num) % HASH_SIZE;
        probe_num++;
    }
	strcpy(table[hashValue].lexeme, lexeme);

	table[hashValue].present = true;
	table[hashValue].symbol = symbol;
}

// initialize keywordTable with keywords
void init_hash_table() { 
    // printf("inserting\n");
	insert( "integer" , INTEGER);
	insert( "real" , REAL);
	insert( "boolean" , BOOLEAN);
	insert( "of" , OF);
	insert( "array" , ARRAY);
	insert( "start" , START);
	insert( "end" , END);
	insert( "declare" , DECLARE);
    insert( "module" , MODULE);
    insert( "driver" , DRIVER);
	insert( "program" , PROGRAM);
	insert( "get_value" , GET_VALUE);
	insert( "print" , PRINT);
	insert( "use" , USE);
	insert( "with" , WITH);
	insert( "parameters" , PARAMETERS);
	insert( "true" , TRUE);
	insert( "false" , FALSE);
	insert( "takes" , TAKES);
	insert( "input" , INPUT);
	insert( "returns" , RETURNS);
	insert( "AND" , AND);
	insert( "OR" , OR);
	insert( "for" , FOR);
	insert( "in" , IN);
	insert( "switch" , SWITCH);
	insert( "case" , CASE);
	insert( "break" , BREAK);
	insert( "default" , DEFAULT);
	insert( "while" , WHILE);
    // left TAGGED, UNION and RECORD

    // printf("Values inserted");
}

token_name searchLookupTable(char *lexeme){
	int hashValue = hash(lexeme);
	int probe_num = 1;
	while (table[hashValue].present == true) {
		if(strcmp(table[hashValue].lexeme, lexeme) == 0) {
			return table[hashValue].symbol;
		}	
		hashValue = (hashValue + probe_num*probe_num) % HASH_SIZE;
        probe_num++;
    }
	
	return ID;
}

