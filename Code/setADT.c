/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#include "setADT.h"

/**
 * @brief initializes the set
 * 
 * initialize each element to be 0
 */
void set_init(set s) 
{
  for (int i = 0; i < SET_SIZE; i++) 
  {
    s[i] = 0;
  }
}

/**
 * @brief checks the presence of element in set
 * 
 */
bool set_find_elem(set s, int num) 
{
  return ( (s[num / ULL_NUM_BITS] & (((1ULL << (ULL_NUM_BITS - 1)) >> (num % ULL_NUM_BITS)))) != 0 );
}

/**
 * @brief Remove an element from the set
 * 
 */
void set_remove_elem(set s, int num) 
{
  s[num / ULL_NUM_BITS] &= (~((1ULL << (ULL_NUM_BITS - 1)) >> (num % ULL_NUM_BITS)));
}

/**
 * @brief Add element to the set
 * 
 */
void set_add_elem(set s, int num) 
{
  s[num / ULL_NUM_BITS] |= ((1ULL << (ULL_NUM_BITS - 1)) >> (num % ULL_NUM_BITS));
}

/**
 * @brief Finds the union of two sets
 * 
 * @param dest union value
 */
void set_union(set dest, set first, set second) {
  for (int j = 0; j < SET_SIZE; j++) 
  {
    dest[j] = first[j] | second[j];
  }
}

/**
 * @brief Finds the intersection of two sets
 * 
 * @param dest intersection value
 */
void set_intersection(set dest, set first, set second) {
  for (int j = 0; j < SET_SIZE; j++) {
    dest[j] = first[j] & second[j];
  }
}

/**
 * @brief Checks if a set is superset of another set
 * 
 */
bool is_superset(set first, set second) 
{
  for (int j = 0; j < SET_SIZE; j++) {
    if ((first[j] & second[j]) != second[j]) {
      return false;
    }
  }
  return true;
}