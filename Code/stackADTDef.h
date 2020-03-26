/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef STACK_ADT_H
#define STACK_ADT_H

#include "parserDef.h"
#include "treeADT.h"

#define INIT_STACK_SIZE 200

/**
 * @brief Stack implemented as dynamic array
 *
 */
typedef struct
{
    tree_node **arr;
    int top;
    int size;
} stack;

#endif