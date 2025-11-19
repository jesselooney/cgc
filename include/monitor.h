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
    monitor_write_state();
}

void monitor_write_state()
{

}


#endif
