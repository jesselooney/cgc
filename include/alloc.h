#ifndef ALLOC_H
#define ALLOC_H

#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"

//===============================================
// Members
//===============================================

#define ALLOC_POOL_SIZE_EXP 16
#define ALLOC_POOL_SIZE (1 << ALLOC_POOL_SIZE_EXP)

static void *ALLOC_HEAP_START = NULL;
static void *ALLOC_HEAP_TOP = NULL;

typedef struct {
    size_t block_size;
    uint8_t data[ALLOC_POOL_SIZE - sizeof(size_t)];
} pool_t;

void alloc_init();
void *alloc_new(size_t size);
void alloc_del_by_id(pool_t * pool, size_t block_id);
void alloc_set_mark_bit(void *block);
bool alloc_is_heap_ptr(void *ptr);

static void *_alloc_new_pool(size_t size);
static void _alloc_set_free_bit_by_id(pool_t * pool, size_t block_id);
static void _alloc_clear_free_bit(void *block);

typedef struct {
    int exp;
    // Should always be 2^exp.
    int pow;
} _log2_ceil_return_t;

static _log2_ceil_return_t _alloc_log2_ceil(size_t size);

//===============================================
// Definitions
//===============================================

#define ALLOC_HEAP_SIZE_EXP 30
#define ALLOC_HEAP_SIZE (1 << ALLOC_HEAP_SIZE_EXP)
#define ALLOC_MIN_BLOCK_COUNT_EXP 3
#define ALLOC_MAX_BLOCK_COUNT_EXP 6
#define ALLOC_MIN_BLOCK_SIZE_EXP 5
#define ALLOC_MAX_BLOCK_SIZE_EXP (ALLOC_POOL_SIZE_EXP - ALLOC_MIN_BLOCK_COUNT_EXP)

static void *ALLOC_FREE_LISTS[ALLOC_MAX_BLOCK_SIZE_EXP] = { NULL };

#define BITVEC_SIZE(block_size) ((ALLOC_POOL_SIZE) / (block_size) / 8)
#define HEADER_SIZE(block_size) (sizeof(size_t) + 2 * BITVEC_SIZE(block_size))
#define BLOCKS_PER_HEADER(block_size) ((HEADER_SIZE(block_size) + (block_size) - 1) / (block_size))

// TODO: Use a mask instead of shifting for GET_POOL.
#define GET_POOL(BLOCK) (((BLOCK) >> ALLOC_POOL_SIZE_EXP) << ALLOC_POOL_SIZE_EXP)
#define BLOCK_ID(POOL, BLOCK) (((BLOCK) - (POOL)) / POOL->block_size)
#define GET_BLOCK(POOL, BLOCK_ID) (((void*) (POOL)) + ((BLOCK_ID) * POOL->block_size))

#define GET_BIT(BYTE, INDEX) ((BYTE >> INDEX) & 1)
#define SET_BIT(BYTE_PTR, INDEX) (*BYTE_PTR |= 1 << INDEX)
#define CLEAR_BIT(BYTE_PTR, INDEX) (*BYTE_PTR &= ~(1 << INDEX))

//  alloc_init()
//      initialize a giant slab of memory for us to parse up. start the heap at the first 
//      pool aligned address and bump the top up from there
void alloc_init()
{
    log_info("alloc_init()");
    void *ptr = mmap(NULL, ALLOC_HEAP_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        log_error("call to mmap failed with errno == %d", errno);
        exit(errno);
    }

    log_debug("ptr == %p", (void *) ptr);
    ALLOC_HEAP_START = (void *) (ptr + ((intptr_t) ptr % ALLOC_POOL_SIZE));
    ALLOC_HEAP_TOP = ALLOC_HEAP_START;
    log_debug("ALLOC_HEAP_START == ALLOC_HEAP_TOP == %p", ALLOC_HEAP_TOP);
    log_info("alloc_init() == void");
}

//  alloc_new(size_t)
//      allocate memory
//      rounded up to occupy a block of the next power of 2 size
void *alloc_new(size_t size)
{
    log_info("alloc_new(%ld)", size);
    _log2_ceil_return_t log2_ceil = _alloc_log2_ceil(size);
    int index = log2_ceil.exp;
    int block_size = log2_ceil.pow;
    log_debug("index == %d", index);
    log_debug("block_size == %d", block_size);

    // if smaller than smallest block, over-allocate in the smallest block size
    if (index < ALLOC_MIN_BLOCK_SIZE_EXP) {
        index = ALLOC_MIN_BLOCK_SIZE_EXP;
        log_info("block size too small; clamping index to %d", index);
    }
    // Fail if requested size is larger than the largest size class.
    if (index > ALLOC_MAX_BLOCK_SIZE_EXP) {
        log_info("block size too large; returning NULL");
        // TODO: Add large object handling. 
        return NULL;
    }
    // Get the next block in the free list for this size class.
    void *block = ALLOC_FREE_LISTS[index];

    // If there is no next block, create one by allocating a pool.
    if (block == NULL) {
        log_info("no free block in size class; allocating a new pool");
        void *head_block = _alloc_new_pool(block_size);
        if (head_block == NULL) {
            log_info("failed to allocate new pool");
            log_info("alloc_new(...) == NULL");
            return NULL;        // Failed to allocate the pool.
        }
        block = head_block;
    }
    log_debug("block == %p", block);

    // Pull `block` off the free list.
    void *next_block = *((void **) block);
    log_debug("next_block == %p", next_block);
    ALLOC_FREE_LISTS[index] = next_block;

    // Null out `block`'s pointer into the free list so we don't give the user
    // easy access to the free list.
    *((void **) block) = NULL;

    // Mark `block` as occupied before returning it to the user.
    _alloc_clear_free_bit(block);

    log_info("alloc_new(...) == %p", block);
    return block;
}

void alloc_del_by_id(pool_t *pool, size_t block_id)
{
    log_info("alloc_del_by_id(%p, %lu)", pool, block_id);
    void *block = GET_BLOCK(pool, block_id);
    _alloc_set_free_bit_by_id(pool, block_id);

    // Get the index of the free list `block` belongs in.
    _log2_ceil_return_t log2_ceil = _alloc_log2_ceil(pool->block_size);
    int index = log2_ceil.exp;

    // Insert `block` at the front of the correct free list.
    *((void **) block) = ALLOC_FREE_LISTS[index];
    ALLOC_FREE_LISTS[index] = block;
}

// alloc_set_mark_bit(void*)
//      we have found an object that has a live pointer to it. pass that pointer to the object 
//      to set its mark bit in the bitmap
void alloc_set_mark_bit(void *block)
{
    // block is theoretically the pointer to the start of a heap allocated block, but it 
    // could point inside of one and the rounding should work out, probably.
    pool_t *pool = (pool_t *) GET_POOL((intptr_t) block);
    size_t block_id = BLOCK_ID((intptr_t) pool, (intptr_t) block);
    uint8_t *resident_byte =
        &pool->data[BITVEC_SIZE(pool->block_size) + (block_id / 8)];
    SET_BIT(resident_byte, block_id % 8);
}

bool alloc_is_heap_ptr(void *ptr)
{
    return ALLOC_HEAP_START <= ptr && ptr < ALLOC_HEAP_TOP;
}

//  _alloc_new_pool(size_t)
//      if the ll of blocks in a size class is empty, we need to initialize a new set of 
//      blocks in a new pool. 
//      return NULL if no pool could be allocated
//      return a pointer to the block at the head of the linked list of blocks on success
static void *_alloc_new_pool(size_t block_size)
{
    log_info("_alloc_new_pool(%ld)", block_size);

    log_debug("ALLOC_HEAP_TOP == %p", ALLOC_HEAP_TOP);
    log_debug("ALLOC_POOL_SIZE == %d", ALLOC_POOL_SIZE);
    log_debug("ALLOC_HEAP_START == %p", ALLOC_HEAP_START);
    log_debug("ALLOC_HEAP_SIZE == %d", ALLOC_HEAP_SIZE);
    if (ALLOC_HEAP_TOP + ALLOC_POOL_SIZE >
        ALLOC_HEAP_START + ALLOC_HEAP_SIZE) {
        log_info("cannot allocate new pool");
        log_info("_alloc_new_pool(...) == NULL");
        return NULL;
    }

    pool_t *pool = (pool_t *) ALLOC_HEAP_TOP;
    ALLOC_HEAP_TOP += ALLOC_POOL_SIZE;
    log_debug("ALLOC_HEAP_TOP == %p", ALLOC_HEAP_TOP);

    pool->block_size = block_size;
    log_debug("pool == %p", pool);
    log_debug("pool->block_size == %ld", pool->block_size);

    // Set all the free bits. This includes the bits corresponding to blocks
    // this header occupies. Since we never add those to the free list, this
    // ensures they will never be touched in any way hereafter.
    log_debug("BITVEC_SIZE(block_size) == %ld", BITVEC_SIZE(block_size));
    memset(pool->data, ~0, BITVEC_SIZE(block_size));

    void *pool_start = pool;
    void *pool_end = pool_start + ALLOC_POOL_SIZE;
    void *pool_header_end = pool_start + HEADER_SIZE(block_size);
    void *prev = NULL;
    void *block = pool_start + block_size;
    log_info("dividing pool into blocks");
    log_debug("pool_end == %p", pool_end);
    log_debug("pool_header_end == %p", pool_header_end);
    for (; block < pool_end; block += block_size) {
        if (block < pool_header_end) {
            continue;
        }

        *((void **) block) = prev;
        prev = block;
    }

    log_info("_alloc_new_pool(...) == %p", prev);
    return prev;
}

static void _alloc_set_free_bit_by_id(pool_t *pool, size_t block_id)
{
    uint8_t *resident_byte = &pool->data[block_id / 8];
    SET_BIT(resident_byte, block_id % 8);
}

static void _alloc_clear_free_bit(void *block)
{
    log_info("_alloc_clear_free_bit(%p)", block);

    pool_t *pool = (pool_t *) GET_POOL((intptr_t) block);
    log_debug("pool == %p", pool);
    log_debug("pool->block_size = %ld", pool->block_size);

    size_t block_id = BLOCK_ID((intptr_t) pool, (intptr_t) block);
    uint8_t *resident_byte = &pool->data[block_id / 8];
    CLEAR_BIT(resident_byte, block_id % 8);

    log_info("_alloc_clear_free_bit(...) == void");
}

// Quick and dirty method to find the smallest number pow such that 2^pow >= size.
// WARN: not actually guaranteed to work for all values of type size_t, since there
// might be overflow issues.
// TODO: Short circuit once pow gets too big.
static _log2_ceil_return_t _alloc_log2_ceil(size_t size)
{
    int exp = 0;
    int pow = 1;
    while (pow < size) {
        exp++;
        pow *= 2;
    }

    _log2_ceil_return_t ret;
    ret.exp = exp;
    ret.pow = pow;

    return ret;
}

#endif
