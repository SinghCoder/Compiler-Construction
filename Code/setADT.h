/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef SET_ADT_H
#define SET_ADT_H

#include "setADTDef.h"

void set_init(set s);

bool set_find_elem(set s, int num);

void set_remove_elem(set s, int num);

void set_add_elem(set s, int num);

void set_union(set dest, set first, set second);

void set_intersection(set dest, set first, set second);

bool is_superset(set first, set second);

#endif