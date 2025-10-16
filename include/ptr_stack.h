#ifndef PTR_STACK_H
#define PTR_STACK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "arc.h"
#include "stack.h"

stack_t *PTR_STACK = NULL;

// ==============================================
// Signatures
// ==============================================

void ptr_stack_init();

void ptr_stack_push(void **p);

void ptr_stack_scope_start(int arg_count, ...);

void ptr_stack_scope_end();

// ==============================================
// Definitions
// ==============================================

void ptr_stack_init()
{
    PTR_STACK = stack_init();
    printf("\tDEBUG: ptr_stack_init(): Created PTR_STACK at %p\n",
           PTR_STACK);
}

void ptr_stack_push_break()
{
    stack_push(PTR_STACK, NULL);
    printf
        ("\tDEBUG: ptr_stack_push_break(): Pushed null to PTR_STACK at index %d\n",
         PTR_STACK->top);
}

void ptr_stack_push(void **p)
{
    if (p == NULL) {
        printf
            ("\tERROR: ptr_stack_push(): pushed a null pointer to the scope stack\n");
        exit(1);
    }
    stack_push(PTR_STACK, p);
    printf
        ("\tDEBUG: ptr_stack_push(): Pushed address %p to PTR_STACK at index %d\n",
         p, PTR_STACK->top);
}

void **ptr_stack_pop()
{
    void **p = stack_pop(PTR_STACK);
    printf
        ("\tDEBUG: ptr_stack_pop(): Popped address %p from PTR_STACK at index %d\n",
         p, PTR_STACK->top + 1);
    return p;
}

void ptr_stack_scope_start(int arg_count, ...)
{
    printf
        ("\tDEBUG: ptr_stack_scope_start(): Entering with %d arguments...\n",
         arg_count);

    va_list args;
    va_start(args, arg_count);

    ptr_stack_push_break();
    for (int i = 0; i < arg_count; i++) {
        void **p = va_arg(args, void **);
        ptr_stack_push(p);
    }

    va_end(args);
}

void ptr_stack_scope_end()
{
    printf("\tDEBUG: ptr_stack_scope_end(): Entering...\n");
    void **p;
    while (1) {
        p = stack_pop(PTR_STACK);
        if (p == NULL)
            break;
#ifdef GC_ARC
        arc_delete(p);
#endif
    }
}

#endif
