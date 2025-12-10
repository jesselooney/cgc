#include <stdio.h>
#include <time.h>
#include "cgc.h"


typedef struct list {
    int head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t *a, void (*f)(void *))
{
    (*f) (a->tail);
}

void iterate() {
    cgc_scope_start();

    cgc_scope_declare(list_t *, a);
    cgc_alloc(&a, list_t);
    
    cgc_scope_declare(list_t*, b);
    cgc_alloc(&b, list_t);
    
    cgc_scope_declare(list_t*, c);
    //cgc_alloc(&c, list_t);

    cgc_assign(&a->tail, b);
    cgc_assign(&b->tail, c);

    cgc_scope_end();
    cgc_free(a);
    cgc_free(b);
    //cgc_free(c);
    return;
}

int main() {
    cgc_scope_start();

    int total_iterations  = 10000000;
    int target_recordings = 1000;
    int threshold = total_iterations / target_recordings;

    for (int i = 0; i < total_iterations; i++) {
        iterate();
        if (i % threshold == 0) {
            cgc_monitor_write_user("progress,%d\n", i);
        }
    }
    cgc_scope_end();
    return 0;
}