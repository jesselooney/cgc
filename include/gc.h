#ifndef GC_H
#define GC_H

// ==============================================
// Members
// ==============================================

// #define gc_register(P)
// #define gc_alloc(P, T)
// #define gc_assign(P, Q)
// #define gc_deregister(P)

size_t GC_TOTAL_PTR_ASSIGNS = 0;

// ==============================================
// ARC
// ==============================================
#ifdef GC_ARC

#ifdef GC_TRC
exit(1);
#endif

#include "arc.h"
#define gc_register(P) arc_register(P)
#define gc_alloc(P, T) arc_alloc(P, sizeof(T), T ## __map_ptrs)
#define _gc_assign(P, Q) arc_assign(P, Q)
#define gc_deregister(P) arc_deregister(P)

#endif

// ==============================================
// TRC
// ==============================================
#ifdef GC_TRC

#define gc_register(P)
#define gc_deregister(P)
#include "trc.h"
#define gc_alloc(P, T) trc_alloc(P, sizeof(T), T ## __map_ptrs)
#define _gc_assign(P, Q) ((*P) = (Q))

#endif

#define gc_assign(P, Q) \
{ \
    _gc_assign(P, Q); \
    GC_TOTAL_PTR_ASSIGNS += 1; \
    monitor_write_state();\
}

#endif
