#include <stdint.h>
#include <stdio.h>

#define GC_ARC
#include "gc.h"

typedef struct list {
    uint8_t head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t *a, void (*f)(void *))
{
    printf("Destroying list with head = %d\n", a->head);
    (*f) (&a->tail);
}

int main()
{
    gc_init();

    gc_scope_start();
    gc_scope_declare(list_t*, a);
    gc_scope_declare(list_t*, b);
    gc_scope_declare(list_t*, c);

    gc_alloc(&a, list_t);
    a->head = 0;
    a->tail = NULL;

    gc_alloc(&b, list_t);
    b->head = 1;
    b->tail = NULL;

    gc_alloc(&c, list_t);
    c->head = 2;
    c->tail = NULL;

    gc_assign(&a->tail, b);
    //gc_assign(&b->tail, c);
    gc_assign(&c->tail, a);

    //gc_register(a, c);

    /* puts("Deleting c");
    arc_delete(&c);
    puts("Deleting b");
    arc_delete(&b);
    puts("Deleting a");
    arc_delete(&a); */

    gc_scope_end();

    return 0;
}
