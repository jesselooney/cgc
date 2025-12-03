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
    cgc_alloc(&c, list_t);

    cgc_assign(&a->tail, b);
    cgc_assign(&b->tail, c);
    cgc_assign(&c->tail, a);

    cgc_scope_end();
    return;
}

int main() {

    FILE *f;

    time_t rawtime;
    struct tm *info;
    char buf[27];
    time(&rawtime);
    info = localtime(&rawtime); 
    strftime(buf, sizeof(buf), "reports/%y%m%d-%H%M%S.csv", info);

    f = fopen(buf, "w");
    cgc_monitor_register_outfile(f);

    cgc_init();
    cgc_scope_start();

    for (int i = 0; i < 1000; i++) {
        iterate();
        cgc_monitor_write_heapstate();
    }

    cgc_scope_end();

    fclose(f);

    return 0;
}