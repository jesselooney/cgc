#include <stddef.h>

// macro

void arc_alloc(void **p, size_t size);

void arc_assign(void **p, void *q);

void arc_register(int numargs, ...);

void arc_delete(void **p);

void arc_set_ptr_finder(void *p, void (*ptr_finder)(void*));

