#ifndef CGC_H
#define CGC_H

#include "gc.h"
#include "macros.h"
#include "monitor.h"
#include "ptr_stack.h"

// ==============================================
// Members
// ==============================================

// #define cgc_alloc(P, T)
// #define cgc_assign(P, Q)
// #define cgc_scope_declare(T, N)
// #define cgc_collect()
// #define cgc_scope_start(...)
// #define cgc_scope_end()

static void _cgc_init();
static void _cgc_end();
static void _cgc_collect();

// ==============================================
// Macros
// ==============================================

#define cgc_alloc(P, T) gc_alloc(P, T)
#define cgc_assign(P, Q) gc_assign(P, Q)
#define cgc_array(P, T, N) gc_array(P, T, N)
#define cgc_free(P) gc_free(P)
#define cgc_scope_declare(T, N) T N = NULL; ptr_stack_push(&N);
#define cgc_collect() _cgc_collect()

#define cgc_scope_start(...) \
{\
    if (PTR_STACK == NULL) {\
        _cgc_init();\
    }\
    ptr_stack_scope_start(ARG_COUNT(__VA_ARGS__), ## __VA_ARGS__);\
}

#define cgc_scope_end() \
{\
    ptr_stack_scope_end();\
    if (PTR_STACK->top == 0) {\
        _cgc_end();\
    }\
}

// ==============================================
// Definitions
// ==============================================

static void _cgc_init()
{
    ptr_stack_init();
    monitor_init();
#ifdef GC_ARC
    arc_init();
#endif
#ifdef GC_TRC
    trc_init();
#endif
#ifdef GC_NOP
    nop_init();
#endif
}

static void _cgc_end()
{
    monitor_end();
}

static void _cgc_collect()
{
#ifdef GC_TRC
    trc_collect();
#endif
}

#endif
