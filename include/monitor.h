#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "alloc.h"

// ==============================================
// Members
// ==============================================

static FILE *outfile;
static struct timespec start;
static bool enabled = false;

#define bufsize 10000
char logbuf[bufsize + 1000];
int bufpos = 0;

// granularity
#define GRAN_HEAPSTATE 100000
int c_heapstate = 0;

void cgc_monitor_write_heapstate();

void monitor_init();
void monitor_end();
void monitor_write_heapstate();

void _monitor_buffer_write(char* line, ...);

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
        #ifdef GC_ARC
        char* gc_select = "arc"; 
        #endif
        #ifdef GC_TRC
        char* gc_select = "trc"; 
        #endif
        
        fprintf(outfile, "metadata,%s\n", gc_select);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    }
}

void monitor_end()
{
    if (enabled) {
        fprintf(outfile, "%s", logbuf);
        fflush(outfile);
        fclose(outfile);
    }
}

void _monitor_buffer_vwrite(char* line, va_list args)
{
    // get time elapsed at this write
    struct timespec now;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    time_t nsec_elapsed = (now.tv_sec - start.tv_sec) * 1000000000 +
        (now.tv_nsec - start.tv_nsec);

    // actual printing
    va_list args2;
    va_copy(args2, args);

    int old_bufpos = bufpos;
    bufpos += snprintf(logbuf + bufpos, bufsize - bufpos, "%ld,", nsec_elapsed);
    if ((bufpos += vsnprintf(logbuf + bufpos, bufsize - bufpos, line, args)) < 0) {
        log_error("death and bad (printf encoding error)");
        exit(-1);
    }
    if (bufpos > bufsize) {
        // avoid reading the truncated daata
        logbuf[old_bufpos] = '\0';
        // print the buffer to the file
        fprintf(outfile, "%s\n", logbuf);
        // now print the line that just failed
        bufpos = snprintf(logbuf, bufsize, "##########\n"); // debug to check seams
        bufpos += snprintf(logbuf + bufpos, bufsize - bufpos, "%ld,", nsec_elapsed);
        bufpos += vsnprintf(logbuf + bufpos, bufsize - bufpos, line, args2);
    }

    va_end(args2);
}

void _monitor_buffer_write(char* line, ...)
{
    va_list args;
    va_start(args, line);
    _monitor_buffer_vwrite(line, args);
    va_end(args);
}

void cgc_monitor_write_heapstate()
{
    monitor_write_heapstate();
}

void monitor_write_heapstate()
{
    if (enabled) {
        // granularity
        if (c_heapstate < GRAN_HEAPSTATE) {
            c_heapstate++;
            return;
        } 
        c_heapstate = 0;

        // actually writing

        _monitor_buffer_write(
            "heapstate,%ld,%ld,%ld,%ld\n",
            ALLOC_ALLOCATED_BLOCKS, 
            ALLOC_ALLOCATED_BYTES,
            ALLOC_ALLOCATED_POOLS,
            GC_TOTAL_PTR_ASSIGNS
        );
    }
}

void cgc_monitor_write_user(char* line, ...)
{
    if (enabled) {
        va_list args;
        va_start(args, line);
    
        _monitor_buffer_vwrite(line, args);
    
        va_end(args);
    }
}


#endif
