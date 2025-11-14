#ifndef PTR_STACK_H
#define PTR_STACK_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "gc.h"
#include "stack.h"

// ==============================================
// Members
// ==============================================

#define _PTR_STACK_SENTINEL NULL

stack_t *PTR_STACK = NULL;

void ptr_stack_init();
void ptr_stack_scope_start(int arg_count, ...);
void ptr_stack_push(void **p);
void ptr_stack_scope_end();

static void **_ptr_stack_pop();

// ==============================================
// Definitions
// ==============================================

void ptr_stack_init()
{
    log_info("ptr_stack_init()");
    PTR_STACK = stack_init(100);
    log_info("ptr_stack_init(...) == void");
}

void ptr_stack_scope_start(int arg_count, ...)
{
    log_info("ptr_stack_scope_start(%d, ...)", arg_count);

    va_list args;
    va_start(args, arg_count);

    stack_push(PTR_STACK, _PTR_STACK_SENTINEL);
    for (int i = 0; i < arg_count; i++) {
        void **p = va_arg(args, void **);
        gc_register(*p);
        ptr_stack_push(p);
    }

    va_end(args);

    log_info("ptr_stack_scope_start(...) == void");
}

void ptr_stack_push(void **p)
{
    log_info("ptr_stack_push(%p)", p);

    if (p == _PTR_STACK_SENTINEL) {
        log_warn("Tried to push sentinel value onto PTR_STACK");
        return;
    }
    stack_push(PTR_STACK, p);

    log_info("ptr_stack_push(...) == void");
}

void ptr_stack_scope_end()
{
    log_info("ptr_stack_scope_end()");

    while (true) {
        void **p = _ptr_stack_pop();
        if (p == _PTR_STACK_SENTINEL)
            break;
        gc_deregister(*p);
    }

    log_info("ptr_stack_scope_end(...) == void");
}

static void **_ptr_stack_pop()
{
    log_info("_ptr_stack_pop()");

    void **p = stack_pop(PTR_STACK);

    log_info("_ptr_stack_pop() == %p", p);
    return p;
}

#endif
