#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdio.h>
#include <time.h>
#include "alloc.h"

#define MONITOR_BUFFER_SIZE 1

// ==============================================
// Members
// ==============================================

FILE *outfile;
struct timespec start;

void cgc_monitor_write_state();
void cgc_monitor_register_outfile(FILE * f);

void monitor_init();
void monitor_write_state();

// forward declarations
size_t ALLOC_ALLOCATED_BYTES;
size_t ALLOC_ALLOCATED_BLOCKS;
size_t ALLOC_ALLOCATED_POOLS;

// ==============================================
// Definitions
// ==============================================

void monitor_init()
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
}

void cgc_monitor_register_outfile(FILE * f)
{
    outfile = f;
}

void cgc_monitor_write_state()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    time_t sec_elapsed = (now.tv_sec - start.tv_sec) * 1000000000 +
        (now.tv_nsec - start.tv_nsec);
    fprintf(outfile, "%ld, %ld, %ld\n",
            sec_elapsed, ALLOC_ALLOCATED_BLOCKS, ALLOC_ALLOCATED_BYTES);
    fflush(outfile);
}

void monitor_write_state()
{

}


#endif
