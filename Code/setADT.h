#ifndef SET_ADT_H
#define SET_ADT_H

#include<stdio.h>
#include<stdbool.h>

#define SET_SIZE 3
#define sz(x) sizeof(x)
#define ULL_NUM_BITS (8 * sz(ull))

typedef unsigned long long int ull;
typedef ull *set;

void set_init(set s);

bool set_find_elem(set s, int num);

void set_remove_elem(set s, int num);

void set_add_elem(set s, int num);

void set_union(set dest, set first, set second);

void set_intersection(set dest, set first, set second);

bool is_superset(set first, set second);

#endif