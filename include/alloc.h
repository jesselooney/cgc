#ifndef ALLOC_H
#define ALLOC_H

#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "monitor.h"

//===============================================
// Members
//===============================================

// active parameters
int ALLOC_POOL_SIZE_EXP;
int ALLOC_HEAP_SIZE_EXP;

size_t ALLOC_POOL_SIZE;
size_t ALLOC_HEAP_SIZE;
int ALLOC_MIN_BLOCK_COUNT_EXP;
int ALLOC_MAX_BLOCK_COUNT_EXP;
int ALLOC_MIN_BLOCK_SIZE_EXP;
int ALLOC_MAX_BLOCK_SIZE_EXP;

// todo, 32?
static void *ALLOC_FREE_LISTS[32] = { NULL };

static void *ALLOC_HEAP_START = NULL;
static void *ALLOC_HEAP_TOP = NULL;

typedef struct {
    size_t block_size;
    uint8_t data[];
} pool_t;

typedef void block_t;

void alloc_init();
void *alloc_new(size_t size);
void alloc_del();
void alloc_del_by_id(pool_t * pool, size_t block_id);
bool alloc_get_mark_bit(void *block);
void alloc_set_mark_bit(block_t * block);
bool alloc_is_heap_ptr(void *ptr);

static void *_alloc_new_pool(size_t size);
static void _alloc_set_free_bit_by_id(pool_t * pool, size_t block_id);
static void _alloc_clear_free_bit(block_t * block);

size_t bitvec_size(size_t block_size);
size_t header_size(size_t block_size);
pool_t *get_pool(block_t * block);
int get_block_id(pool_t * pool, block_t * block);
block_t *get_block_by_id(pool_t * pool, size_t block_id);

int get_bit(uint8_t byte, int index);
void set_bit(uint8_t * byte_ptr, int index);
void clear_bit(uint8_t * byte_ptr, int index);

// log2 ceil
typedef struct {
    int exp;
    // Should always be 2^exp.
    int pow;
} _log2_ceil_return_t;

static _log2_ceil_return_t _alloc_log2_ceil(size_t size);

// monitor members
size_t ALLOC_ALLOCATED_BYTES = 0;
size_t ALLOC_ALLOCATED_BLOCKS = 0;
size_t ALLOC_ALLOCATED_POOLS = 0;

//===============================================
// Definitions
//===============================================

//  alloc_init()
//      initialize a giant slab of memory for us to parse up. start the heap at the first 
//      pool aligned address and bump the top up from there
void alloc_init()
{
    log_info("alloc_init()");

    const char* env_pool_exp = getenv("CGC_POOL_EXP");
    if (env_pool_exp == NULL) {
        ALLOC_POOL_SIZE_EXP = 16;
    } else {
        ALLOC_POOL_SIZE_EXP = atoi(env_pool_exp);
    }

    const char* env_heap_exp = getenv("CGC_HEAP_EXP");
    if (env_pool_exp == NULL) {
        ALLOC_HEAP_SIZE_EXP = 30;
    } else {
        ALLOC_HEAP_SIZE_EXP = atoi(env_heap_exp);
    }

    ALLOC_POOL_SIZE = 1 << ALLOC_POOL_SIZE_EXP;
    ALLOC_HEAP_SIZE = 1 << ALLOC_HEAP_SIZE_EXP;
    ALLOC_MIN_BLOCK_COUNT_EXP = 3;
    ALLOC_MAX_BLOCK_COUNT_EXP = 6;
    ALLOC_MIN_BLOCK_SIZE_EXP = 5;
    ALLOC_MAX_BLOCK_SIZE_EXP = ALLOC_POOL_SIZE_EXP - ALLOC_MIN_BLOCK_COUNT_EXP;

    void *ptr = mmap(NULL, ALLOC_HEAP_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        log_error("call to mmap failed with errno == %d", errno);
        exit(errno);
    }

    log_debug("ptr == %p", (void *) ptr);
    ALLOC_HEAP_START = (void *) (ptr + (ALLOC_POOL_SIZE - ((intptr_t) ptr % ALLOC_POOL_SIZE)));
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
    block_t *block = ALLOC_FREE_LISTS[index];

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
    block_t *next_block = *((block_t **) block);
    log_debug("next_block == %p", next_block);
    ALLOC_FREE_LISTS[index] = next_block;

    // Initialize the block's memory for safety.
    memset(block, 0, block_size);

    // Mark `block` as occupied.
    _alloc_clear_free_bit(block);

    // mon ============
    ALLOC_ALLOCATED_BLOCKS += 1;
    ALLOC_ALLOCATED_BYTES += block_size;
    monitor_write_state();
    // mon ============

    log_info("alloc_new(...) == %p", block);
    return block;
}

void alloc_del(block_t * block)
{
    log_info("alloc_del(%p)", block);

    pool_t *pool = get_pool(block);
    size_t block_id = get_block_id(pool, block);
    alloc_del_by_id(pool, block_id);

    log_info("alloc_del(...) == void");
}

void alloc_del_by_id(pool_t * pool, size_t block_id)
{
    log_info("alloc_del_by_id(%p, %lu)", pool, block_id);

    void *block = (void *) get_block_by_id(pool, block_id);
    _alloc_set_free_bit_by_id(pool, block_id);

    // Get the index of the free list `block` belongs in.
    _log2_ceil_return_t log2_ceil = _alloc_log2_ceil(pool->block_size);
    int index = log2_ceil.exp;

    // Insert `block` at the front of the correct free list.
    *((void **) block) = ALLOC_FREE_LISTS[index];
    ALLOC_FREE_LISTS[index] = block;

    // mon ============
    ALLOC_ALLOCATED_BLOCKS -= 1;
    ALLOC_ALLOCATED_BYTES -= pool->block_size;
    monitor_write_state();
    // mon ============

    log_info("alloc_del_by_id(%p, %lu)", pool, block_id);
}

bool alloc_get_mark_bit(void *block)
{
    log_info("alloc_get_mark_bit(%p)", block);

    pool_t *pool = get_pool(block);
    size_t block_id = get_block_id(pool, block);
    uint8_t resident_byte =
        pool->data[bitvec_size(pool->block_size) + (block_id / 8)];
    bool ret = get_bit(resident_byte, block_id % 8);

    log_info("alloc_get_mark_bit(...) == %d", ret);
    return ret;
}

// alloc_set_mark_bit(void*)
//      we have found an object that has a live pointer to it. pass that pointer to the object 
//      to set its mark bit in the bitmap
void alloc_set_mark_bit(block_t * block)
{
    log_info("alloc_set_mark_bit(%p)", block);
    // block is theoretically the pointer to the start of a heap allocated block, but it 
    // could point inside of one and the rounding should work out, probably.
    pool_t *pool = get_pool(block);
    size_t block_id = get_block_id(pool, block);
    uint8_t *resident_byte =
        &pool->data[bitvec_size(pool->block_size) + (block_id / 8)];
    set_bit(resident_byte, block_id % 8);
    log_info("alloc_set_mark_bit(...) == void");
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
    log_debug("bitvec_size(block_size) == %ld", bitvec_size(block_size));
    memset(pool->data, ~0, bitvec_size(block_size));

    void *pool_start = pool;
    void *pool_end = pool_start + ALLOC_POOL_SIZE;
    void *pool_header_end = pool_start + header_size(block_size);
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
    ALLOC_ALLOCATED_POOLS += 1;
    return prev;
}

static void _alloc_set_free_bit_by_id(pool_t * pool, size_t block_id)
{
    uint8_t *resident_byte = &pool->data[block_id / 8];
    set_bit(resident_byte, block_id % 8);
}

static void _alloc_clear_free_bit(block_t * block)
{
    log_info("_alloc_clear_free_bit(%p)", block);

    pool_t *pool = get_pool(block);
    log_debug("pool == %p", pool);
    log_debug("pool->block_size = %ld", pool->block_size);

    size_t block_id = get_block_id(pool, block);
    uint8_t *resident_byte = &pool->data[block_id / 8];
    clear_bit(resident_byte, block_id % 8);

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


size_t bitvec_size(size_t block_size)
{
    return ALLOC_POOL_SIZE / block_size / 8;
}

size_t header_size(size_t block_size)
{
    return sizeof(size_t) + 2 * bitvec_size(block_size);
}

pool_t *get_pool(block_t * block)
{
    return (pool_t *) (((intptr_t) block >> ALLOC_POOL_SIZE_EXP) <<
                       ALLOC_POOL_SIZE_EXP);
}

int get_block_id(pool_t * pool, block_t * block)
{
    return ((intptr_t) block - (intptr_t) pool) / pool->block_size;
}

block_t *get_block_by_id(pool_t * pool, size_t block_id)
{
    return (block_t *) (((intptr_t) pool) + (block_id * pool->block_size));
}

int get_bit(uint8_t byte, int index)
{
    return (byte >> index) & 1;
}

void set_bit(uint8_t * byte_ptr, int index)
{
    *byte_ptr |= 1 << index;
}

void clear_bit(uint8_t * byte_ptr, int index)
{
    *byte_ptr &= ~(1 << index);
}


#endif
