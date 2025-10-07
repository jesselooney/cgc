#include <stdint.h>
#include <stdio.h>

//#include "arc.h"
#define GC_ARC
#include "gc.h"

typedef struct list {
    uint8_t head;
    struct list *tail;
} list_t;

void list_map_ptrs(list_t *a, void (*f)(void*))
{
    printf("Destroying list with head = %d\n", a->head);
    (*f)(&a->tail);
}

int main()
{
    list_t *a, *b, *c;

    gc_alloc(&a, sizeof(list_t));
    gc_set_map_ptrs(a, list_map_ptrs);
    a->head = 0;
    a->tail = NULL;

    gc_alloc(&b, sizeof(list_t));
    gc_set_map_ptrs(b, list_map_ptrs);
    b->head = 1;
    b->tail = NULL;

    gc_alloc(&c, sizeof(list_t));
    gc_set_map_ptrs(c, list_map_ptrs);
    c->head = 2;
    c->tail = NULL;

    gc_assign(&a->tail, b);
    //gc_assign(&b->tail, c);
    gc_assign(&c->tail, a);

    //gc_register(a, c);

    puts("Deleting c");
    arc_delete(&c);
    puts("Deleting b");
    arc_delete(&b);
    puts("Deleting a");
    arc_delete(&a);

    return 0;
}
