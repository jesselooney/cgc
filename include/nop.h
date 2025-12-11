#ifndef NOP_H
#define NOP_H

#include <stdlib.h>
#include "debug.h"

void nop_init()
{

}


void nop_alloc(void **p, size_t size)
{
    void *space = malloc(size);
    if (space == NULL) {
        log_error("death and bad (failed to malloc)");
    }
    *p = space;
    log_trace("a %p", *p);
}

void nop_free(void *p)
{
    log_trace("f %p", p);
    free(p);
}

#endif
