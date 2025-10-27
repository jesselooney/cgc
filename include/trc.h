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

typedef struct {
    size_t block_size;
    silly_t bitmaps;
} _trc_header_t;

void *HEAP_START = NULL;

void trc_init();

void trc_alloc(void **p, size_t size,
               void (*map_ptrs)(void *, void(*f)(void *)));

static void _trc_collect();

static void _trc_mark();

static void _trc_sweep();

static _trc_header_t *_trc_get_header_ptr(void *p);

static bool _trc_is_heap_ptr(void *p);

// ==============================================
// Definitions
// ==============================================

void trc_init() 
{
    HEAP_START = alloc_init();
}

void trc_alloc(void **p, size_t size,
               void (*map_ptrs)(void *, void(*f)(void *)))
{
    void* space = alloc_new(size + sizeof(intptr_t));
    if (space == NULL) {
        _trc_collect();
        space = alloc_new(size + sizeof(intptr_t));
        if (space == NULL) {
            // kill and death
            exit(1);
        }
    }
    *space = map_ptrs; // this looks wrong
    *p = space + sizeof(intptr_t);
    log_trace("a %p", *p);
}

void _trc_collect() 
{
    _trc_mark();
    _trc_sweep();
}

// sfrom the root set, dfs using destructors 
void _trc_mark() 
{
    // prepare dfs stack 
    stack_t *to_visit;
    to_visit = stack_init();

    // initialize dfs stack with root set
    void **item;
    size_t stack_top = PTR_STACK->top;
    for (int i = 0; i < stack_top; i++)
    {
        item = (PTR_STACK->items)[i];
        if (_trc_is_heap_ptr(item))
        {
            // ignore sentinels
            stack_push(to_visit, item);
        }
    }

    // perform dfs on this stack
    void **visiting;
    while ((visiting = stack_pop(to_visit)) != NULL)
    {
        // when you visit an allocation
        //      calculate block number based on 0x10000 distance
        //      flip marked bit in the marked bit vector
        // after dfs, zero out the marked bit vector
    }
}

void _trc_sweep() 
{
    // retrieve top of heap from the allocator
    // in 0x10000 steps,
    // read the size of the blocks
    // calculate bit vector sizes
    // load each bit vector in
    // ~ both then & 
    // any 1's correspond to blocks to free
    // iterate through each; free and link back to the free list

    // need access to:
    //  top of the heap from the allocator
    //  pointer to the top-level size class list
}

static _trc_header_t *_trc_get_header_ptr(void *p)
{
    return (_trc_header_t *) (((uintptr_t) p) >> 16 ) << 16;
}

static bool _trc_is_heap_ptr(void *p)
{
    // TODO: need this?
    return p != NULL;
}

#endif
