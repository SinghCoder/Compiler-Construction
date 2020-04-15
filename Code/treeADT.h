/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef TREE_ADT_H
#define TREE_ADT_H

#include "treeADTDef.h"

tree_node *create_tree_node();

void add_child(tree_node *parent, tree_node *child);

tree_node* delete_child(tree_node* parent, tree_node* prev, tree_node *child);

tree_node *get_nth_child(tree_node *root, int n);
#endif