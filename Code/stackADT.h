#ifndef stack_H
#define stack_H

#include "parserDef.h"
#include "treeADT.h"

#define MAX_PARSING_NODES 200

typedef struct
{
    tree_node **arr;
    int top;
    int size;
} stack;

stack *stack_init();

void push(stack *stack_ptr, tree_node* node);

tree_node* top(stack *stack_ptr);

tree_node* pop(stack* stack_ptr);

#endif