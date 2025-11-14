#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdio.h>
#include <time.h>
#include "alloc.h"

FILE *outfile;
struct timespec start;

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
    time_t sec_elapsed = (now.tv_sec - start.tv_sec) * 1000000 +
        (now.tv_nsec - start.tv_nsec) / 1000;
    fprintf(outfile, "%ld, %d, %ld\n",
            sec_elapsed,
            ALLOC_NUM_ALLOCATED_BLOCKS, ALLOC_SIZE_OF_ALLOCATED_MEMORY);
    fflush(outfile);
}

#endif
