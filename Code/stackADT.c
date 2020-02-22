#include "stackADT.h"
#include <stdio.h>
#include <stdlib.h>

stack *stack_init() {
  stack *stack_ptr = (stack *)malloc(sizeof(stack));
  stack_ptr->size = 200;
  stack_ptr->arr = (tree_node **)malloc(sizeof(tree_node *) * stack_ptr->size);
  stack_ptr->top = 0;
  return stack_ptr;
}

void push(stack *stack_ptr, tree_node *node) {
  if (top == (stack_ptr->size)) {
    stack_ptr->size *= 2;
    realloc(stack_ptr->arr, sizeof(tree_node *) * stack_ptr->size);
  }
  stack_ptr->arr[stack_ptr->top] = node;
  stack_ptr->top++;
  return stack_ptr;
}

tree_node *pop(stack *stack_ptr) {
  if (stack_ptr->top == 0) {
    // perror("Tried to pop empty stack\n");
    return NULL;
  }

  stack_ptr->top--;
  return stack_ptr->arr[stack_ptr->top];
}

tree_node *top(stack *stack_ptr) {
  if (stack_ptr->top == 0) {
    // perror("Tried to find top of empty stack\n");
    return NULL;
  } else {
    return stack_ptr->arr[stack_ptr->top - 1];
  }
}