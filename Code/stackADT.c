/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#include "stackADT.h"
#include <stdio.h>
#include <stdlib.h>

stack *stack_init() 
{
  stack *stack_ptr = (stack *)malloc(sizeof(stack));
  if(stack_ptr == NULL)
  {
    perror("stack init error.. memory not available\n");
  }
  stack_ptr->size = MAX_PARSING_NODES;
  stack_ptr->arr = (tree_node **)malloc( sizeof(tree_node *) * (stack_ptr->size) );
  if(stack_ptr->arr == NULL)
  {
    perror("stack array member init error.. memory not available\n");
  }
  stack_ptr->top = 0;
  return stack_ptr;
}

void push(stack *stack_ptr, tree_node *node) 
{
  if (stack_ptr->top == (stack_ptr->size)) 
  {
    stack_ptr->size *= 2;
    tree_node **tmp = realloc(stack_ptr->arr, sizeof(tree_node *) * stack_ptr->size);
    if(tmp == NULL)
    {
      perror("Error in doubling stack's size: realloc failed\n");
    }
    else
    {
      stack_ptr->arr = tmp;
    }
  }
  stack_ptr->arr[stack_ptr->top] = node;
  stack_ptr->top++;
  // return stack_ptr;
}

tree_node *pop(stack *stack_ptr) {
  if (stack_ptr->top == 0) 
  {
    // perror("Tried to pop empty stack\n");
    return NULL;
  }

  stack_ptr->top--;
  return stack_ptr->arr[stack_ptr->top];
}

tree_node *top(stack *stack_ptr) {
  if (stack_ptr->top == 0) 
  {
    // perror("Tried to find top of empty stack\n");
    return NULL;
  } 
  else 
  {
    return stack_ptr->arr[stack_ptr->top - 1];
  }
}