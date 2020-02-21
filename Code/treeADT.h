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
} tree_node;

tree_node *create_tree_node();



#endif