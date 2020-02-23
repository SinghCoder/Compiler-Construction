#include "treeADT.h"
#include <stdlib.h>
#include <stdio.h>

tree_node *create_tree_node() {
  tree_node *node = (tree_node *)malloc(sizeof(tree_node));
  if(node == NULL)
  {
    perror("tree_node allocation error..\n");
    exit(0);
  }
  node->parent = NULL;
  node->sibling = NULL;
  node->leftmost_child = NULL;
  node->rightmost_child = NULL;
  return node;
}

void add_child(tree_node *parent, tree_node *child) 
{
  // if(child->sym.is_terminal == true)
  //   printf("\nChild added : %s\n", terminal_string[child->sym.t]);
  // else
  //   printf("\nChild added : %s\n", non_terminal_string[child->sym.nt]);
  if (parent->rightmost_child == NULL) 
  {
    parent->leftmost_child = child;
    parent->rightmost_child = child;
  } 
  else 
  {
    parent->rightmost_child->sibling = child;
    parent->rightmost_child = child;
  }
}
