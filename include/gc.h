#ifndef GC_H
#define GC_H

// Should only be called on argument lists whose elements are pointers.
#define ARG_COUNT(...) (sizeof((void*[]) {__VA_ARGS__}) / sizeof(void*))

#ifdef GC_ARC
#include "arc.h"

#define gc_alloc(P, SIZE) arc_alloc(P, SIZE)
#define gc_assign(P, Q) arc_assign(P, Q)
#define gc_register(...) arc_register(ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define gc_set_map_ptrs(P, MAP_PTRS) arc_set_map_ptrs(P, MAP_PTRS)
#endif


#include "ptr_stack.h"

#define gc_scope_init() ptr_stack_init()

#define gc_scope_start(...) ptr_stack_scope_start(ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
#define gc_scope_end() ptr_stack_scope_end()

#endif
