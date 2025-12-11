#ifndef TRC_H
#define TRC_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "debug.h"
#include "alloc.h"
#include "ptr_stack.h"
#include "stack.h"

// ==============================================
// Members
// ==============================================

stack_t *SEARCH_STACK = NULL;

typedef struct {
    void (*map_ptrs)(void *, void (*f)(void *));
} _trc_header_t;

void trc_init();
void trc_alloc(void **p, size_t size,
               void (*map_ptrs)(void *, void (*f)(void *)));
void trc_collect();

static void _trc_mark();
static void _trc_sweep();
static bool _trc_is_heap_ptr(void *p);
static void _trc_push_to_search_stack(void *p);

// ==============================================
// Definitions
// ==============================================


void trc_init()
{
    alloc_init();
}

void trc_alloc(void **p, size_t size,
               void (*map_ptrs)(void *, void (*f)(void *)))
{
    if(p == NULL) {
        log_error
            ("Passed NULL to trc_alloc (did you mean to pass &p instead of p?)");
        exit(1);
    }
    _trc_header_t *header = alloc_new(size + sizeof(intptr_t));
    if (header == NULL) {
        // todo: this should call logging code for collecting?
        trc_collect();
        header = alloc_new(size + sizeof(intptr_t));
        if (header == NULL) {
            log_error("Failed to allocate a block of size %ld", size);
            exit(1);
        }
    }
    header->map_ptrs = map_ptrs;
    *p = header + 1;
    log_trace("a %p", *p);
}

void trc_collect()
{
    _trc_mark();
    _trc_sweep();
}

// from the root set, dfs using map_ptrs 
void _trc_mark()
{
    log_info("_trc_mark()");

    log_info("pushing root set onto search stack");
    // prepare dfs stack 
    SEARCH_STACK = stack_init(PTR_STACK->top);

    // initialize dfs stack with root set
    void **item;
    size_t stack_top = PTR_STACK->top;
    for (int i = 0; i < stack_top; i++) {
        item = (PTR_STACK->items)[i];
        log_info("processing ptr stack item %p", item);
        // Ignore sentinels and pointers to non-heap objects.
        if (item != PTR_STACK_SENTINEL && _trc_is_heap_ptr(*item)) {
            log_debug("pushing pointer %p", *item);
            stack_push(SEARCH_STACK, *item);
        }
    }

    // Perform DFS on starting from the root set stack.
    log_info("starting DFS");
    while (SEARCH_STACK->top > 0) {
        _trc_header_t *header = get_start_of_block(stack_pop(SEARCH_STACK));

        void *visiting = (void *) (header + 1);

        log_info("visiting heap object at %p", visiting);

        // Mark that we found a path from the root set to `visiting`.
        alloc_set_mark_bit(visiting);

        log_debug("header->map_ptrs == %p", header->map_ptrs);
        if (header->map_ptrs != NULL) {
            log_info("calling map_ptrs to find child pointers");
            // Push the pointers contained in `visiting` onto the search stack.
            (*header->map_ptrs) (visiting, _trc_push_to_search_stack);
            log_info("finished call to map_ptrs");
        }
    }

    log_info("trc_mark(...) == void");
}

static void _trc_push_to_search_stack(void *p)
{
    log_info("_trc_push_to_search_stack(%p)", p);

    if (_trc_is_heap_ptr(p)) {
        if (!alloc_get_mark_bit(p)) {
            log_info("pushing pointer %p", p);
            stack_push(SEARCH_STACK, (void **) p);
        } else {
            log_info("already marked; skipping");
        }
    } else {
        log_info("not a heap pointer; skipping");
    }

    log_info("_trc_push_to_search_stack(...) == void");
}

void _trc_sweep()
{
    log_info("_trc_sweep()");

    for (void *curr_pool = ALLOC_HEAP_START; curr_pool < ALLOC_HEAP_TOP;
         curr_pool += ALLOC_POOL_SIZE) {
        pool_t *pool = curr_pool;
        log_info("sweeping pool at %p", pool);
        // bitvec_size yields the number of bytes in the bitvec for this pool.
        for (int i = 0; i < bitvec_size(pool->block_size); i++) {
            // Get the ith byte of the bitvec containing the free and mark bits.
            uint8_t is_free = pool->data[i];
            uint8_t *is_marked =
                &pool->data[bitvec_size(pool->block_size) + i];
            uint8_t should_free = ~(is_free | *is_marked);

            // Clear the mark bits now that we've used them.
            *is_marked = 0;

            // For each bit, free the corresponding block if applicable.
            for (int j = 0; j < 8; j++) {
                if (get_bit(should_free, j)) {
                    alloc_del_by_id(pool, i * 8 + j);
                    log_trace("f %p",
                              ((_trc_header_t
                                *) (get_block_by_id(pool,
                                                    i * 8 + j))) + 1);
                }
            }
        }
    }

    log_info("_trc_sweep(...) == void");
}

static bool _trc_is_heap_ptr(void *p)
{
    log_info("_trc_is_heap_ptr(%p)", p);
    bool ret = alloc_is_heap_ptr(p);
    log_info("_trc_is_heap_ptr(...) == %d", ret);
    return ret;
}

#endif
