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
#include <string.h>
/**
 * @brief Create a tree node object
 *
 * @return pointer to created node
 */

tree_node *create_tree_node() {
  tree_node *node = (tree_node *)malloc(sizeof(tree_node));
  if (node == NULL) {
    perror("tree_node allocation error..\n");
    exit(0);
  }

  node->visited = false;
  node->parent = NULL;
  node->sibling = NULL;
  node->leftmost_child = NULL;
  node->rightmost_child = NULL;
  node->next = NULL;
  node->node_inh = NULL;
  node->node_syn = NULL;
  strcpy(node->token.str, "");
  return node;
}

/**
 * @brief Add the child as rightmost_child to given node
 *
 */

void add_child(tree_node *parent, tree_node *child) {
  if (parent->rightmost_child == NULL) {
    parent->leftmost_child = child;
    parent->rightmost_child = child;
  } else {
    parent->rightmost_child->sibling = child;
    parent->rightmost_child = child;
  }
}

tree_node *delete_child(tree_node *parent, tree_node *prev, tree_node *child) {
  if (child == parent->leftmost_child) {
    parent->leftmost_child = child->sibling;
    free(child);
    return parent->leftmost_child;
  } else if (child == parent->rightmost_child) {
    prev->sibling = NULL;
    parent->rightmost_child = prev;
    free(child);
    return NULL;
  } else {
    if (child == NULL)
      printf("child");
    prev->sibling = child->sibling;
    free(child);
    return prev->sibling;
  }
}