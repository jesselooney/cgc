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

#ifdef GC_TRC
// death and dying
exit(1);
#endif

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

#include "trc.h"
#define gc_register(P) 
#define gc_alloc(P, T) trc_alloc(P, sizeof(T), T ## __map_ptrs)
#define gc_assign(P, Q) ((*P) = (Q))
#define gc_deregister(P)

#endif

#endif
