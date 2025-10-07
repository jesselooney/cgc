void gc_scope_start(int numargs, ...);
void gc_scope_end();

void gc_register(int numargs, ...);
void gc_alloc(void **p, size_t size);
void gc_set_ptr_finder(void *p, void (*ptr_finder)(void*));
void gc_assign(void **p, void *q);
