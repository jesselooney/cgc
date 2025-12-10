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

int monitor_buffer_size;
char* monitor_buffer;
int monitor_buffer_position = 0;

// granularity
int monitor_granularity_heapstate;
int monitor_count_heapstate = 0;

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
        
        /////// open outpath
        outfile = fopen(outpath, "w");
        if (outfile == NULL) {
            perror("death and bad (error opening outfile csv)");
            exit(1);
        }
        /////// 

        /////// read env vars
        const char* monbuf_size = getenv("CGC_MONBUF_SIZE");
        if (monbuf_size) {
            monitor_buffer_size = atoi(monbuf_size);
        } else {
            monitor_buffer_size = 10000;
        }
        monitor_buffer = malloc(sizeof(char) * (monitor_buffer_size + 1000));

        const char* mon_gran_heapstate = getenv("CGC_MON_GRAN_HEAPSTATE");
        if (mon_gran_heapstate) {
            monitor_granularity_heapstate = atoi(mon_gran_heapstate);
        } else {
            monitor_granularity_heapstate = 100000;
        }
        ///////
        
        /////// metadata
        char* gc_select;
        #ifdef GC_ARC
        gc_select = "arc"; 
        #endif
        #ifdef GC_TRC
        gc_select = "trc"; 
        #endif
        #ifdef GC_NOP
        gc_select = "nop";
        #endif
        assert(gc_select != NULL);
        fprintf(outfile, "metadata,%s\n", gc_select);
        ///////
        
        /////// record start time
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        /////// 
    }
}

void monitor_end()
{
    if (enabled) {
        fprintf(outfile, "%s", monitor_buffer);
        fflush(outfile);
        fclose(outfile);
    }
}

// debugging thing basically
void monitor_flush_buf()
{
    fprintf(outfile, "%s", monitor_buffer);
    fflush(outfile);
    monitor_buffer_position = 0;
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

    int old_bufpos = monitor_buffer_position;
    monitor_buffer_position += snprintf(monitor_buffer + monitor_buffer_position, monitor_buffer_size - monitor_buffer_position, "%ld,", nsec_elapsed);
    if ((monitor_buffer_position += vsnprintf(monitor_buffer + monitor_buffer_position, monitor_buffer_size - monitor_buffer_position, line, args)) < 0) {
        log_error("death and bad (printf encoding error)");
        exit(-1);
    }
    if (monitor_buffer_position > monitor_buffer_size) {
        // avoid reading the truncated data
        monitor_buffer[old_bufpos] = '\0';
        // print the buffer to the file
        fprintf(outfile, "%s\n", monitor_buffer);
        // now print the line that just failed
        monitor_buffer_position = snprintf(monitor_buffer, monitor_buffer_size, "##########\n"); // debug to check seams
        monitor_buffer_position += snprintf(monitor_buffer + monitor_buffer_position, monitor_buffer_size - monitor_buffer_position, "%ld,", nsec_elapsed);
        monitor_buffer_position += vsnprintf(monitor_buffer + monitor_buffer_position, monitor_buffer_size - monitor_buffer_position, line, args2);
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
        if (monitor_count_heapstate < monitor_granularity_heapstate) {
            monitor_count_heapstate++;
            return;
        } 
        monitor_count_heapstate = 0;

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
