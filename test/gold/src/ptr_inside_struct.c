#include <stdint.h>

#include "cgc.h"
#include "debug.h"

typedef struct list {
    uint8_t head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t *a, void (*f)(void *))
{
    (*f)(a->tail);
}

int main()
{
    cgc_scope_start();

    cgc_scope_declare(list_t *, tail);

    cgc_scope_start();

    // Make a two-list chain a --> b.
    cgc_scope_declare(list_t *, a);
    cgc_alloc(&a, list_t);
    a->head = 0;
    a->tail = NULL;

    cgc_scope_declare(list_t *, b);
    cgc_alloc(&b, list_t);
    b->head = 1;
    b->tail = NULL;

    cgc_assign(&a->tail, b);

    // `tail` is a pointer _inside_ of `a`.
    cgc_assign(&tail, &a->tail);

    // Drop `a` and `b`.
    cgc_scope_end();
    cgc_collect();

    // The objects should be kept alive here by `tail`.
    log_trace("allocated objects should not have been collected yet");

    // Drop `tail`, making the objects garbage.
    cgc_scope_end();
    cgc_collect();

    log_trace("allocated objects should have been collected by now");

    return 0;
}
