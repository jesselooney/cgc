#ifndef ARC_H
#define ARC_H

#include <stddef.h>

void arc_alloc(void **p, size_t size);

void arc_assign(void **p, void *q);

void arc_register(int arg_count, ...);

void arc_delete(void **p);

void arc_set_map_ptrs(
        void *p,
        void (*map_ptrs)(void*, void (*f)(void*))
    );

#endif

