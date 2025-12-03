#include <stdint.h>
#include <stdio.h>

#include "cgc.h"

typedef struct list {
    uint8_t head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t * a, void (*f)(void *))
{
    (*f) (a->tail);
}

void init_arr(list_t ***p) {
    cgc_scope_start(&p);

    cgc_scope_declare(list_t **, arr);
    cgc_array(&arr, list_t *, 3);

    cgc_alloc(&arr[0], list_t);
    arr[0]->head = 0;
    arr[0]->tail = NULL;

    cgc_alloc(&arr[1], list_t);
    arr[1]->head = 1;
    arr[1]->tail = NULL;

    cgc_alloc(&arr[2], list_t);
    arr[2]->head = 2;
    arr[2]->tail = NULL;

    cgc_assign(p, arr);

    cgc_scope_end();
}

int main()
{
    cgc_scope_start();
   
    cgc_scope_declare(list_t **, arr);
    init_arr(&arr);
    
    // At this point, the list_t's have gone out of scope, but arr should be
    // keeping them alive.

    cgc_collect();

    cgc_scope_end();

    // Now the list_t's should get cleaned up.

    cgc_collect();

    return 0;
}
