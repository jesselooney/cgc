#ifndef GC_H
#define GC_H

// Should only be called on argument lists whose elements are pointers.
#define ARG_COUNT(...) (sizeof((void*[]) {__VA_ARGS__}) / sizeof(void*))

#if GC_ARC
    #include "arc.h"

    #define gc_alloc(P, T) arc_alloc(P, sizeof(T), T ## __map_ptrs)
    #define gc_assign(P, Q) arc_assign(P, Q)
#endif

#if GC_TRC
#endif

#include "ptr_stack.h"

#define gc_scope_start(...) ptr_stack_scope_start(ARG_COUNT(__VA_ARGS__), ## __VA_ARGS__)
#define gc_scope_declare(T, N) T N; ptr_stack_push(&N);
#define gc_scope_end() ptr_stack_scope_end()

#define gc_init() ptr_stack_init()

#endif
