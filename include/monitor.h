#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdio.h>
#include <time.h>
#include "alloc.h"

#define MONITOR_BUFFER_SIZE 1

typedef struct stat {
    time_t elapsed_nsec;

    size_t allocated_bytes;
    size_t allocated_blocks;
    size_t allocated_pools;

    size_t total_ptr_assigns;
} stat_t;

typedef struct stat_collections {
    #ifdef GC_ARC
    time_t elapsed_nsec;
    time_t duration_nsec;
    #endif

    #ifdef GC_TRC
    time_t elapsed_nsec;
    time_t duration_nsec;

    size_t number_marked;
    size_t number_freed;
    int number_root_set_ptrs;
    #endif
} stat_collections_t;

typedef struct all_stat {
    stat_t stats[MONITOR_BUFFER_SIZE];
    stat_collections_t col_stats[MONITOR_BUFFER_SIZE];
} all_stat_t;

//

FILE *outfile;

struct timespec start;
struct timespec start_col;

all_stat_t stat_buffer;

//

void cgc_monitor_write_state();
void cgc_monitor_register_outfile(FILE * f);

void monitor_init();
void monitor_write_state();
void monitor_write_collection_state_start();
void monitor_write_collection_state_end();

void _monitor_flush_stat_buffer();

//

void monitor_init()
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
}

// todo: env var
void cgc_monitor_register_outfile(FILE * f)
{
    outfile = f;
}

// todo: not this
void cgc_monitor_write_state()
{
    monitor_write_state();
    /* struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    time_t sec_elapsed = (now.tv_sec - start.tv_sec) * 1000000 +
        (now.tv_nsec - start.tv_nsec) / 1000;
    fprintf(outfile, "%ld, %d, %ld\n",
            sec_elapsed,
            ALLOC_NUM_ALLOCATED_BLOCKS, ALLOC_SIZE_OF_ALLOCATED_MEMORY);
    fflush(outfile); */
}

void monitor_write_state()
{

}

void monitor_write_collection_state_start()
{

}

#ifdef GC_ARC
void monitor_write_collection_state_end()
{

}
#endif

#ifdef GC_TRC
void monitor_write_collection_state_end()
{
    
}
#endif

void _monitor_flush_stat_buffer()
{

}


#endif
