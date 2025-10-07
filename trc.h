#include <stddef.h>

// macro

void trc_alloc(void **p, size_t size);

void trc_set_ptr_finder(void *p, void (*ptr_finder)(void*));

