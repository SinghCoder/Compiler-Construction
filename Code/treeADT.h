#ifndef TREE_ADT_H
#define TREE_ADT_H

#include<stdbool.h>
#include "parserDef.h"

typedef struct TREENODE{
	struct TREENODE *parent;
	struct TREENODE *sibling;
	struct TREENODE *leftmost_child;
	struct TREENODE *rightmost_child;
	symbol sym;
	TOKEN token;
} tree_node;

tree_node *create_tree_node();

void add_child(tree_node *parent, tree_node *child);

#endif