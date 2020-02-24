/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#include "treeADT.h"
#include <stdio.h>
#include <stdlib.h>

tree_node *create_tree_node() {
  tree_node *node = (tree_node *)malloc(sizeof(tree_node));
  if (node == NULL) 
  {
    perror("tree_node allocation error..\n");
    exit(0);
  }
  node->parent = NULL;
  node->sibling = NULL;
  node->leftmost_child = NULL;
  node->rightmost_child = NULL;
  node->token.str = NULL;
  return node;
}

void add_child(tree_node *parent, tree_node *child) {
  if (parent->rightmost_child == NULL) {
    parent->leftmost_child = child;
    parent->rightmost_child = child;
  } else {
    parent->rightmost_child->sibling = child;
    parent->rightmost_child = child;
  }
}
