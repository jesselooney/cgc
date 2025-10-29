#ifndef ALLOC_H
#define ALLOC_H

#include <sys/mman.h>
#include <stddef.h>

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

static ALLOC_HEAP_SIZE_EXP = 30;
static ALLOC_HEAP_SIZE = 1 << ALLOC_HEAP_SIZE_EXP;
static ALLOC_POOL_SIZE_EXP = 16;
static ALLOC_POOL_SIZE = 1 << ALLOC_POOL_SIZE_EXP;
static ALLOC_MIN_BLOCK_COUNT_EXP = 3;
static ALLOC_MAX_BLOCK_COUNT_EXP = 6;

static ALLOC_MAX_BLOCK_SIZE_EXP = ALLOC_POOL_SIZE_EXP - ALLOC_MIN_BLOCK_COUNT_EXP;

// // for a given block_size_exp:
// bitvec_size_exp = alloc_pool_size_exp - 3 - block_size_exp;
// header_size = sizeof(size_t) + (1 << (bitvec_size_exp + 1));
// block_size = (1 << block_size_exp);
// blocks_per_header = (header_size + block_size - 1) / block_size; // = ceil(header_size / block_size)
// block_count = 1 << (alloc_pool_size_exp - block_size_exp);
// 
// // 0 <= i < block_count
// bool get_bit(pool, block_size_exp, i) {
//     pool + sizeof(size_t) + i / bit_vec_size
//     
// }
//

#define BITVEC_SIZE(block_size) (alloc_pool_size / block_size / 8)
#define HEADER_SIZE(block_size) (sizeof(size_t) + 2 * bitvec_size(block_size))
#define BLOCKS_PER_HEADER(block_size) ((header_size(block_size) + block_size - 1) / block_size)

// TODO: Use a mask.
#define GET_POOL(BLOCK) ((BLOCK >> ALLOC_POOL_SIZE_EXP) << ALLOC_POOL_SIZE_EXP)
#define BLOCK_ID(POOL, BLOCK) ((BLOCK - POOL) / POOL->block_size)

#define GET_BIT(BYTE, I) ((BYTE >> I) & 1)
//#define SET_BIT(

// block is theoretically the pointer to the start of a heap allocated block, but it 
// could point inside of one and the rounding should work out, probably.
void alloc_set_mark_bit(void *block) {
    pool_t *pool = GET_POOL(block);
    size_t block_id = BLOCK_ID(pool, block);
    uint8_t *resident_byte = &pool->data[BITVEC_SIZE(pool->block_size) + block_id / 8];
    SET_BIT(resident_byte, block_id % 8);
}


typedef struct {
    size_t block_size;
    uint8_t[alloc_pool_size - sizeof(size_t)] data;
} pool_t;

void alloc_init() {
    void *ptr = mmap(NULL, ALLOC_HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);

   ALLOC_HEAP_START = ptr + (ptr % alloc_pool_size);

   ALLOC_HEAP_TOP = ALLOC_HEAP_START;
}

// Return value of NULL indicates failure
void *alloc_new_pool(size_t block_size) {
    if (ALLOC_HEAP_TOP + ALLOC_POOL_SIZE > ALLOC_HEAP_START + ALLOC_HEAP_SIZE)
        return NULL;

    pool_t *pool = alloc_heap_top;
    alloc_heap_top += alloc_pool_size;

    pool->block_size = block_size;
    // Set all the free bits. This includes the bits corresponding to blocks
    // this header occupies. Since we never add those to the free list, this
    // ensures they will never be touched in any way hereafter.
    memset(pool->data, ~0, bitvec_size(block_size));

    void *pool_start = pool;

    for (void *block = pool_start + block_size; block < pool_start + alloc_pool_size; block += block_size) {
        if (i < pool_start + header_size(block_size)) continue;

        prev_block
        *((intptr_t) block) = N
    }

    return pool_start;
}

#endif
