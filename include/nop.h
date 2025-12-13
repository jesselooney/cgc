#ifndef NOP_H
#define NOP_H

#include <stdlib.h>
#include "debug.h"
#include "alloc.h"

void nop_init()
{
    alloc_init();
}


void nop_alloc(void **p, size_t size)
{
    void *space = alloc_new(size);
    if (space == NULL) {
        log_error("death and bad (failed to alloc space for %ld)", size);
        exit(-1);
    }
    *p = space;
    log_trace("a %p", *p);
}

void nop_free(void *p)
{
    log_trace("f %p", p);
    alloc_del(p);
}

#endif
