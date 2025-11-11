#ifndef CGC_H
#define CGC_H

#include "gc.h"
#include "macros.h"

#define cgc_alloc(P, T) gc_alloc(P, T)
#define cgc_assign(P, Q) gc_assign(P, Q)

#include "ptr_stack.h"

#define cgc_scope_start(...) ptr_stack_scope_start(ARG_COUNT(__VA_ARGS__), ## __VA_ARGS__)
#define cgc_scope_declare(T, N) T N = NULL; ptr_stack_push(&N);
#define cgc_scope_end() ptr_stack_scope_end()

#define cgc_init() _cgc_init()
#define cgc_collect() _cgc_collect()

#endif


static void _cgc_init()
{
    ptr_stack_init();
#ifdef GC_ARC
    arc_init();
#endif
#ifdef GC_TRC
    trc_init();
#endif
}


static void _cgc_collect()
{
#ifdef GC_TRC
    trc_collect();
#endif
}
