#ifndef TRC_H
#define TRC_H

#include <stddef.h>

void trc_alloc(void **p, size_t size);

void trc_set_ptr_finder(void *p, void (*ptr_finder)(void *));

#endif
