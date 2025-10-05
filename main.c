#include <stdint.h>
#include <stdio.h>

#include "arc.h"

typedef struct list {
    uint8_t head;
    struct list *tail;
} list_t;

void destroy_list(list_t *a)
{
    printf("Destroying list with head = %d\n", a->head);
    arc_delete(&a->tail);
}

int main()
{
    list_t *a, *b, *c;

    arc_alloc(&a, sizeof(list_t));
    arc_set_destructor(a, destroy_list);
    a->head = 0;
    a->tail = NULL;

    arc_alloc(&b, sizeof(list_t));
    arc_set_destructor(b, destroy_list);
    b->head = 1;
    b->tail = NULL;

    arc_alloc(&c, sizeof(list_t));
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
