
#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdio.h>

unsigned short *stack;
int sp;
int stack_size;

int init_stack(size_t);
int push(unsigned short);
unsigned short pop();
int stack_terminate();

#endif //STACK_H