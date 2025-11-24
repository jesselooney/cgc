#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdio.h>
#include <time.h>
#include "alloc.h"

#define MONITOR_BUFFER_SIZE 100000

// ==============================================
// Members
// ==============================================

static FILE *outfile;
static struct timespec start;
static bool enabled = false;

#define bufsize 100000
char logbuf[bufsize];
int bufpos = 0;

void cgc_monitor_write_state();

void monitor_init();
void monitor_end();
void monitor_write_state();

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
        //setvbuf(outfile, NULL, _IOFBF, 100000000);
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

void _monitor_buffer_write(char* line, ...)
{
    va_list args, args2;
    va_start(args, line);
    va_copy(args2, args);

    if ((bufpos += vsnprintf(logbuf + bufpos, bufsize - bufpos, line, args)) < 0) {
        log_error("death and bad (printf encoding error)");
        exit(-1);
    }
    if (bufpos > bufsize) {
        fprintf(outfile, "%s\n", logbuf);
        memset(logbuf, 0, sizeof(logbuf));
        if ((bufpos = vsnprintf(logbuf, bufsize, line, args2)) < 0) {
            log_error("death and bad (printf encoding error)");
            exit(-1);
        }
    }

    va_end(args);
    va_end(args2);
}

void cgc_monitor_write_state()
{
    monitor_write_state();
}

void monitor_write_state()
{
    if (enabled) {
        struct timespec now;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
        time_t nsec_elapsed = (now.tv_sec - start.tv_sec) * 1000000000 +
            (now.tv_nsec - start.tv_nsec);

        _monitor_buffer_write(
            "%ld, %ld, %ld, %ld, %ld\n",
            nsec_elapsed, 
            ALLOC_ALLOCATED_BLOCKS, 
            ALLOC_ALLOCATED_BYTES,
            ALLOC_ALLOCATED_POOLS,
            GC_TOTAL_PTR_ASSIGNS
        );
    }
}


#endif
