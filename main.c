#include <stdint.h>
#include <stdio.h>

#include "arc.h"

typedef struct list {
    uint8_t head;
    struct list *tail;
} list_t;

void destroy_list(list_t *a) {
    printf("Destroying list with head = %d\n", a->head);
    arc_delete(&a->tail);
}

int main() {
    list_t *a = (list_t*) arc_alloc(sizeof(list_t));
    arc_set_destructor(a, destroy_list);
    a->head = 0;
    a->tail = NULL;

    list_t *b = (list_t*) arc_alloc(sizeof(list_t));
    arc_set_destructor(b, destroy_list);
    b->head = 1;
    b->tail = NULL;
    
    list_t *c = (list_t*) arc_alloc(sizeof(list_t));
    arc_set_destructor(c, destroy_list);
    c->head = 2;
    c->tail = NULL;

    arc_assign(&a->tail, b);
    arc_assign(&b->tail, c);
    //arc_assign(&c->tail, a);

    puts("Deleting c");
    arc_delete(&c);
    puts("Deleting b");
    arc_delete(&b);
    puts("Deleting a");
    arc_delete(&a);

    return 0;
}

