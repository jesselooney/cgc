#ifndef ALLOC_H
#define ALLOC_H

#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

//===============================================
// Members
//===============================================

#define ALLOC_POOL_SIZE_EXP 16
#define ALLOC_POOL_SIZE (1 << ALLOC_POOL_SIZE_EXP)

typedef struct {
    size_t block_size;
    uint8_t data[ALLOC_POOL_SIZE - sizeof(size_t)];
} pool_t;

void alloc_init();
void* alloc_new(size_t size);
void alloc_del(void *ptr);
void alloc_del_by_id(pool_t *pool, size_t block_id);
void alloc_set_mark_bit(void *block);

static int _alloc_log_2_ceil(size_t size);

//===============================================
// Definitions
//===============================================

static void *ALLOC_HEAP_START = NULL;
static void *ALLOC_HEAP_TOP = NULL;

#define ALLOC_HEAP_SIZE_EXP 30
#define ALLOC_HEAP_SIZE (1 << ALLOC_HEAP_SIZE_EXP)
#define ALLOC_MIN_BLOCK_COUNT_EXP 3
#define ALLOC_MAX_BLOCK_COUNT_EXP 6
#define ALLOC_MIN_BLOCK_SIZE_EXP 5
#define ALLOC_MAX_BLOCK_SIZE_EXP (ALLOC_POOL_SIZE_EXP - ALLOC_MIN_BLOCK_COUNT_EXP)

static void *ALLOC_FREE_LISTS[ALLOC_MAX_BLOCK_SIZE_EXP] = {NULL};

#define BITVEC_SIZE(block_size) (ALLOC_POOL_SIZE / block_size / 8)
#define HEADER_SIZE(block_size) (sizeof(size_t) + 2 * BITVEC_SIZE(block_size))
#define BLOCKS_PER_HEADER(block_size) ((HEADER_SIZE(block_size) + block_size - 1) / block_size)

// TODO: Use a mask.
#define GET_POOL(BLOCK) ((BLOCK >> ALLOC_POOL_SIZE_EXP) << ALLOC_POOL_SIZE_EXP)
#define BLOCK_ID(POOL, BLOCK) ((BLOCK - POOL) / POOL->block_size)

#define GET_BIT(BYTE, INDEX) ((BYTE >> INDEX) & 1)
#define SET_BIT(BYTE_PTR, INDEX) (*BYTE_PTR |= 1 << INDEX)


// block is theoretically the pointer to the start of a heap allocated block, but it 
// could point inside of one and the rounding should work out, probably.
void alloc_set_mark_bit(void *block) {
    pool_t *pool = (pool_t *) GET_POOL((intptr_t) block);
    size_t block_id = BLOCK_ID((intptr_t) pool, (intptr_t) block);
    uint8_t *resident_byte = &pool->data[BITVEC_SIZE(pool->block_size) + block_id / 8];
    SET_BIT(resident_byte, block_id % 8);
}

void alloc_init() {
    intptr_t ptr = (intptr_t) mmap(NULL, ALLOC_HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
    ALLOC_HEAP_START = (void *) (ptr + (ptr % ALLOC_POOL_SIZE));
    ALLOC_HEAP_TOP = ALLOC_HEAP_START;
}

// Return value of NULL indicates failure
// Returns a block that is the head of the linked list of blocks in
// this pool.
// TODO: note constraints on allowed values of block_size
void *alloc_new_pool(size_t block_size) {
    if (ALLOC_HEAP_TOP + ALLOC_POOL_SIZE > ALLOC_HEAP_START + ALLOC_HEAP_SIZE)
        return NULL;

    pool_t *pool = (pool_t *) ALLOC_HEAP_TOP;
    ALLOC_HEAP_TOP += ALLOC_POOL_SIZE;

    pool->block_size = block_size;
    // Set all the free bits. This includes the bits corresponding to blocks
    // this header occupies. Since we never add those to the free list, this
    // ensures they will never be touched in any way hereafter.
    memset(pool->data, ~0, BITVEC_SIZE(block_size));

    void *pool_start = pool;
    // TODO: initialize prev to the address of the front of the free list for this size
    // class. Maybe not necessary since we only allocate when there are no other blocks,
    // but worth considering.
    void *prev = NULL;
    void *block = pool_start + block_size;
    for (; block < pool_start + ALLOC_POOL_SIZE; block += block_size) {
        if (block < pool_start + HEADER_SIZE(block_size)) continue;

        *((void**) block) = prev;
        prev = block;
    }

    return block;
}

// Quick and dirty method to find the smallest number pow such that 2^pow >= size.
// WARN: not actually guaranteed to work for all values of type size_t, since there
// might be overflow issues.
// And really we could short circuit once pow gets too big.
typedef struct {
    int exp;
    int pow; // 2^exp
} _log2_ceil_return_t;
static _log2_ceil_return_t _alloc_log2_ceil(size_t size) {
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

void* alloc_new(size_t size) {
    _log2_ceil_return_t log2_ceil = _alloc_log2_ceil(size);
    int index = log2_ceil.exp;
    int block_size = log2_ceil.pow;

    if (index < ALLOC_MIN_BLOCK_SIZE_EXP) index = ALLOC_MIN_BLOCK_SIZE_EXP;
    // Fail if requested size is larger than the largest size class.
    // TODO: Add large object handling.
    if (index > ALLOC_MAX_BLOCK_SIZE_EXP) return NULL;

    // Get the next item of the free list for this size class.
    void *ptr = ALLOC_FREE_LISTS[index];
    if (ptr != NULL) {
        // TODO: un-free the block pointed to by ptr and update the free list.
    } else {
        void *head_block = alloc_new_pool(block_size);
        if (head_block == NULL) return NULL; // Failed to allocate the pool.
        ALLOC_FREE_LISTS[index] = head_block;
    }
}

#endif
