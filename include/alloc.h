#ifndef ALLOC_H
#define ALLOC_H

#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

//===============================================
// Members
//===============================================

//#define ALLOC_HEAP_SIZE

void alloc_init();
void* alloc_new(size_t size);
void alloc_del(void *ptr);
void alloc_del_by_id(pool_t *pool, size_t block_id);
void alloc_set_mark_bit(void *block);

//===============================================
// Definitions
//===============================================

static void *ALLOC_HEAP_START = NULL;
static void *ALLOC_HEAP_TOP = NULL;

#define ALLOC_HEAP_SIZE_EXP 30
#define ALLOC_HEAP_SIZE (1 << ALLOC_HEAP_SIZE_EXP)
#define ALLOC_POOL_SIZE_EXP 16
#define ALLOC_POOL_SIZE (1 << ALLOC_POOL_SIZE_EXP)
#define ALLOC_MIN_BLOCK_COUNT_EXP 3
#define ALLOC_MAX_BLOCK_COUNT_EXP 6
#define ALLOC_MAX_BLOCK_SIZE_EXP (ALLOC_POOL_SIZE_EXP - ALLOC_MIN_BLOCK_COUNT_EXP)

#define BITVEC_SIZE(block_size) (ALLOC_POOL_SIZE / block_size / 8)
#define HEADER_SIZE(block_size) (sizeof(size_t) + 2 * BITVEC_SIZE(block_size))
#define BLOCKS_PER_HEADER(block_size) ((header_size(block_size) + block_size - 1) / block_size)

// TODO: Use a mask.
#define GET_POOL(BLOCK) ((BLOCK >> ALLOC_POOL_SIZE_EXP) << ALLOC_POOL_SIZE_EXP)
#define BLOCK_ID(POOL, BLOCK) ((BLOCK - POOL) / POOL->block_size)

#define GET_BIT(BYTE, I) ((BYTE >> I) & 1)
#define SET_BIT(BYTE_PTR, I) (*BYTE_PTR |= 1 << I)

typedef struct {
    size_t block_size;
    uint8_t data[ALLOC_POOL_SIZE - sizeof(size_t)];
} pool_t;

// block is theoretically the pointer to the start of a heap allocated block, but it 
// could point inside of one and the rounding should work out, probably.
void alloc_set_mark_bit(void *block) {
    pool_t *pool = (pool_t *) GET_POOL((intptr_t) block);
    size_t block_id = BLOCK_ID((intptr_t) pool, (intptr_t) block);
    uint8_t *resident_byte = &pool->data[BITVEC_SIZE(pool->block_size) + block_id / 8];
    SET_BIT(resident_byte, block_id % 8);
}

void alloc_init() {
    intptr_t ptr = mmap(NULL, ALLOC_HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
    ALLOC_HEAP_START = (void *) (ptr + (ptr % ALLOC_POOL_SIZE));
    ALLOC_HEAP_TOP = ALLOC_HEAP_START;
}

// Return value of NULL indicates failure
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

    for (void *block = pool_start + block_size; block < pool_start + ALLOC_POOL_SIZE; block += block_size) {
        if (block < pool_start + HEADER_SIZE(block_size)) continue;

        // *((intptr_t) block) =
    }

    return pool_start;
}

#endif
