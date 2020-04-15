/***************************************
                |GROUP-09|
  Aditya Upadhyay      -   2017A7PS0083P
  Harpider Jot Singh   -   2017A7PS0057P
  Jaladi Lakshmi Teja  -   2017A7PS0068P
  Vishal Mittal        -   2017A7PS0080P
  Yash Vijay           -   2017A7PS0072P
*****************************************/
#ifndef STACK_H
#define STACK_H

#include "stackADTDef.h"

stack *stack_init();

void push(stack *stack_ptr, void* node);

void* top(stack *stack_ptr);

void* pop(stack* stack_ptr);

#endif