#ifndef GC_H
#define GC_H

#include "macros.h"
#include "alloc.h"

// ==============================================
// Members
// ==============================================

// #define gc_register(P)
// #define gc_alloc(P, T)
// #define gc_assign(P, Q)
// #define gc_deregister(P)

size_t GC_TOTAL_PTR_ASSIGNS = 0;

typedef void (*map_ptrs_t)(void *, void (*f)(void *));

void gc_ptr_array_map_ptrs(void *p, void (*f)(void *)) {
    log_info("gc_ptr_array_map_ptrs(%p, %p)", p, f);

    pool_t *pool = get_pool(p);
    size_t block_size = pool->block_size;
    size_t block_id = get_block_id(pool, p);
    block_t *block = get_block_by_id(pool, block_id);

    void **arr = (void **) p;
    size_t arr_size = pool->block_size - ((intptr_t) p - (intptr_t) block);
    size_t arr_len = arr_size / sizeof(void *);

    for (size_t i = 0; i < arr_len; i++) {
        f(arr[i]);
    }

    log_info("gc_ptr_array_map_ptrs(...) == void");
}

map_ptrs_t GC_ARRAY_MAP_PTRS = NULL;

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
#define gc_array(P, T, N) arc_alloc(P, sizeof(T) * N, IS_PTR_TYPE(T) ? gc_ptr_array_map_ptrs: GC_ARRAY_MAP_PTRS)
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
#define gc_array(P, T, N) trc_alloc(P, sizeof(T) * N, IS_PTR_TYPE(T) ? gc_ptr_array_map_ptrs : GC_ARRAY_MAP_PTRS)
#define _gc_assign(P, Q) ((*P) = (Q))

#endif

#define gc_assign(P, Q) \
{ \
    _gc_assign(P, Q); \
    GC_TOTAL_PTR_ASSIGNS += 1; \
    monitor_write_heapstate();\
}

#ifndef GC_ARC
#ifndef GC_TRC
exit(-1)
#endif
#endif

#endif
