/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef TREE_ADT_DEF_H
#define TREE_ADT_DEF_H

#include<stdbool.h>
#include "parserDef.h"

/**
 * @brief n-ary tree node structure 
 * 
 */
typedef struct TREENODE
{
	struct TREENODE *parent;
	struct TREENODE *sibling;
	struct TREENODE *leftmost_child;
	struct TREENODE *rightmost_child;
	symbol sym;
	TOKEN token;
  int rule_num; 
} tree_node;

#endif