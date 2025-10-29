#ifndef TRC_H
#define TRC_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "debug.h"
//#include "alloc.h"
#include "ptr_stack.h"
#include "stack.h"

// ==============================================
// Members
// ==============================================

typedef struct {
    void (*map_ptrs)(void *, void(*f)(void *));
} _trc_header_t;

void *HEAP_START = NULL;

stack_t *SEARCH_STACK = NULL;

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
    _trc_header_t *header = alloc_new(size + sizeof(intptr_t));
    if (header == NULL) {
        _trc_collect();
        header = alloc_new(size + sizeof(intptr_t));
        if (header == NULL) {
            // kill and death
            exit(1);
        }
    }
    header->map_ptrs = map_ptrs;
    *p = header + sizeof(intptr_t);
    log_trace("a %p", *p);
}

void _trc_collect() 
{
    _trc_mark();
    _trc_sweep();
}

// from the root set, dfs using map_ptrs 
void _trc_mark() 
{
    // prepare dfs stack 
    SEARCH_STACK = stack_init();

    // initialize dfs stack with root set
    void **item;
    size_t stack_top = PTR_STACK->top;
    for (int i = 0; i < stack_top; i++)
    {
        item = (PTR_STACK->items)[i];
        // ignore sentinels
        if (_trc_is_heap_ptr(item))
        {
            stack_push(SEARCH_STACK, item);
        }
    }

    void **visiting;
    size_t pool_block_size;
    void (*map_ptrs)(void *, void (void *));
    // perform dfs on this stack
    while (SEARCH_STACK->top > 0)
    {
        // get the next pointer to a heap pointer on the stack
        visiting = stack_pop(SEARCH_STACK);

        // set the corresponding mark bit for this block in that 
        alloc_set_mark_bit(*visiting); // TODO: err rounding? maybe an issue in BLOCK_ID

        // put its pointers on the stack
        map_ptrs = (void (*)(void *, void (void *))) (*visiting - sizeof(intptr_t));
        map_ptrs(*visiting, _trc_putthingonstack);
    }
}

void _trc_putthingonstack(void *p)
{
    stack_push(SEARCH_STACK, (void **) p);
}

void _trc_sweep() 
{
    // retrieve start of heap from the allocator
    pool_t *curr_pool = ALLOC_HEAP_START;
    do
    {
        // read the size of the blocks
        // load each bit vector in
        // ~ both then & 
        // any 1's correspond to blocks to free
        // iterate through each; free and link back to the free list

        for (int i = 0; i < BITVEC_SIZE(curr_pool->block_size); i++) 
        {
            uint8_t free_vec = curr_pool->data[i];
            uint8_t mark_vec = curr_pool->data[i];
            uint8_t to_free = (~ free_vec) & (~ mark_vec);
            for (int j = 0; j < 8; j++) 
            {
                if (GET_BIT(to_free))
                {
                    alloc_del_by_id(curr_pool, i*8 + j);
                }
            }
        }
    }
    // in 0x10000 steps,
    while ((curr_pool += ALLOC_POOL_SIZE) < ALLOC_HEAP_TOP)
}

static _trc_header_t *_trc_get_header_ptr(void *p)
{
    return (_trc_header_t *) (((uintptr_t) p) >> ALLOC_POOL_SIZE_EXP ) << ALLOC_POOL_SIZE_EXP;
}

static bool _trc_is_heap_ptr(void *p)
{
    // TODO: need this?
    return p != NULL;
}

#endif
