#include <stddef.h>

void* arc_alloc(size_t size);

void arc_set_destructor(void *p, void (*destructor)(void*));

void arc_create(void *p);

void arc_delete(void **p);

void arc_assign(void **p, void *q);

