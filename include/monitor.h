#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdio.h>
#include <time.h>
#include "alloc.h"

#define MONITOR_BUFFER_SIZE 100

// ==============================================
// Members
// ==============================================

static FILE *outfile;
static struct timespec start;
static bool enabled = false;

void cgc_monitor_write_state();

void monitor_init();
void monitor_end();
void monitor_write_state();

// forward declarations
size_t ALLOC_ALLOCATED_BYTES;
size_t ALLOC_ALLOCATED_BLOCKS;
size_t ALLOC_ALLOCATED_POOLS;
size_t GC_TOTAL_PTR_ASSIGNS;

// ==============================================
// Definitions
// ==============================================

void monitor_init()
{
    const char* outpath = getenv("CGC_OUTPATH");
    if (outpath) {
        enabled = true;

        outfile = fopen(outpath, "w");
        if (outfile == NULL) {
            perror("death and bad (error opening outfile csv)");
            exit(1);
        }
        //setvbuf(outfile, NULL, _IOFBF, MONITOR_BUFFER_SIZE);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    }
}

void monitor_end()
{
    if (enabled) {
        fflush(outfile);
        fclose(outfile);
    }
}

void cgc_monitor_write_state()
{
    monitor_write_state();
}

void monitor_write_state()
{
    if (enabled) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        time_t nsec_elapsed = (now.tv_sec - start.tv_sec) * 1000000000 +
            (now.tv_nsec - start.tv_nsec);
        fprintf(
            outfile, 
            "%ld, %ld, %ld, %ld, %ld\n",
            nsec_elapsed, 
            ALLOC_ALLOCATED_BLOCKS, 
            ALLOC_ALLOCATED_BYTES,
            ALLOC_ALLOCATED_POOLS,
            GC_TOTAL_PTR_ASSIGNS
        );
        //fflush(outfile);
    }
}


#endif
