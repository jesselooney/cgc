#include <stddef.h>

#include "cgc.h"

typedef struct list {
    int head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t *a, void (*f)(void *))
{
    (*f) (a->tail);
}

int main()
{
    cgc_scope_start();
    cgc_collect();
    cgc_scope_end();
    cgc_collect();
    return 0;
}
