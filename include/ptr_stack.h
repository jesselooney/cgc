#ifndef PTR_STACK_H
#define PTR_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "arc.h"

typedef struct {
    void** arr[1000];
    int    top;
} stack_t;

stack_t *PTR_STACK = NULL;

void  ptr_stack_init() {
    //printf("\tDEBUG: ptr_stack_init(): entry\n");
    PTR_STACK = (stack_t *) (malloc(sizeof(stack_t)));
    PTR_STACK->top = -1;

    printf("\tDEBUG: ptr_stack_init(): Created PTR_STACK at %p\n", PTR_STACK);
}

void  ptr_stack_push_break() {
    PTR_STACK->arr[++PTR_STACK->top] = NULL;
    printf("\tDEBUG: ptr_stack_push_break(): Pushed null to PTR_STACK at index %d\n", PTR_STACK->top);
}

void  ptr_stack_push(void **p) {
    PTR_STACK->arr[++PTR_STACK->top] = p;
    printf("\tDEBUG: ptr_stack_push(): Pushed address %p to PTR_STACK at index %d\n", p, PTR_STACK->top);
}

void** ptr_stack_pop() {
    void** p = PTR_STACK->arr[PTR_STACK->top--];
    printf("\tDEBUG: ptr_stack_pop(): Popped address %p from PTR_STACK at index %d\n", p, PTR_STACK->top+1);
    return p;
}

///

void ptr_stack_scope_start(int arg_count, ...) {
    printf("\tDEBUG: ptr_stack_scope_start(): Entering with %d arguments...\n", arg_count);
    
    va_list args;
    va_start(args, arg_count);
    
    ptr_stack_push_break();
    for (int i = 0; i < arg_count; i++) {
        void **p = va_arg(args, void **);
        ptr_stack_push(p);
    }
    
    va_end(args);
}

void ptr_stack_scope_end() {
    printf("\tDEBUG: ptr_stack_scope_start(): Entering...\n");
    void** p;
    while (1) {
        p = ptr_stack_pop();
        if (p == NULL) break;
        arc_delete(p);
    }
}

#endif
