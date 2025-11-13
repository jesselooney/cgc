#include <stddef.h>

#define GC_ARC
#include "cgc.h"

typedef struct list {
    int head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t * a, void (*f)(void *))
{
    (*f)(a->tail);
}

void fun(list_t * p)
{
    cgc_scope_start(&p);
    cgc_scope_end();
    return;
}

int main()
{
    cgc_init();
    cgc_scope_start();
    // cgc_scope_declare(list_t *, a);
    list_t a;
    //a.head = 0;
    //a.tail = NULL;
    fun(&a);
    cgc_scope_end();
    cgc_collect();
    return 0;
}
