void* arc_alloc(uintptr_t size);

void* arc_set_destructor(void *a, void* destructor);

void arc_assign(void **a, void *b);

void arc_create(void *a);

void arc_delete(void **a);

