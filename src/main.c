#include <stdint.h>
#include <stdio.h>

#define GC_TRC

#include "cgc.h"
#include "debug.h"

typedef struct list {
    uint8_t head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t * a, void (*f)(void *))
{
    printf("Destroying list with head = %d\n", a->head);
    (*f) (a->tail);
}

int main()
{
    cgc_init();

    cgc_scope_start();
    cgc_scope_declare(list_t *, a);
    cgc_scope_declare(list_t *, b);
    cgc_scope_declare(list_t *, c);

    cgc_alloc(&a, list_t);
    a->head = 0;
    a->tail = NULL;

    cgc_alloc(&b, list_t);
    b->head = 1;
    b->tail = NULL;

    cgc_alloc(&c, list_t);
    c->head = 2;
    c->tail = NULL;

    cgc_assign(&a->tail, b);
    //cgc_assign(&b->tail, c);
    cgc_assign(&c->tail, a);

    cgc_scope_end();

    cgc_collect();

    return 0;
}
