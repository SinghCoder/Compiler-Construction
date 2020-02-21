#include "treeADT.h"
#include <stdio.h>

tree_node *create_tree_node() {
  tree_node *node = (tree_node *)malloc(sizeof(tree_node));
  node->parent = NULL;
  node->sibling = NULL;
  node->leftmost_child = NULL;
  node->rightmost_child = NULL;
  return node;
}

void add_child(tree_node *parent, tree_node *child) {

  if (parent->rightmost_child == NULL) {
    parent->leftmost_child = parent->rightmost_child = child;
  } else {
    parent->rightmost_child->sibling = child;
    parent->rightmost_child = child;
  }
}
