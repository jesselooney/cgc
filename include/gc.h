#ifndef GC_H
#define GC_H

// ==============================================
// Members
// ==============================================

// #define gc_register(P)
// #define gc_alloc(P, T)
// #define gc_assign(P, Q)
// #define gc_deregister(P)

// ==============================================
// ARC
// ==============================================
#ifdef GC_ARC

#include "arc.h"
#define gc_register(P) arc_register(P)
#define gc_alloc(P, T) arc_alloc(P, sizeof(T), T ## __map_ptrs)
#define gc_assign(P, Q) arc_assign(P, Q)
#define gc_deregister(P) arc_deregister(P)

#endif

// ==============================================
// TRC
// ==============================================
#ifdef GC_TRC
    // TODO: Prevent specifying more than one of GC_ARC and GC_TRC
#endif

#endif
