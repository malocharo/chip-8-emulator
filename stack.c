#include "stack.h"
#include <stdio.h>

int init_stack(size_t size)
{
    if(size < 1) return -1;
    stack = malloc(sizeof(unsigned short)*size);
    if(stack == NULL) return -2;
    sp = 0;
    stack_size = size;
    for(int i = 0;i<stack_size;i++)
        stack[i] = 0; 
    return 0;
}

int push(unsigned short e)
{
    if(sp == stack_size) return -1;
    stack[sp++] = e;
    printf("add %d at pos %d now sp = %d\n",e,sp-1,sp);
    return 0;
}

unsigned short pop()
{
    if(sp<0) return -1;
    printf("pop %d at pos %d now sp = %d\n",stack[sp],sp-1,sp-1);
    return stack[--sp]; 
}

int stack_terminate()
{
    free(stack);
    return 0;
}

