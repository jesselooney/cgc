#include <stddef.h>

#include "cgc.h"

typedef struct list {
    int head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t *list, void (*f)(void *))
{
    (*f) (list->tail);
}

void push(list_t **list, int x) {
    cgc_scope_start(&list);

    cgc_scope_declare(list_t *, new_node);
    cgc_alloc(&new_node, list_t);

    new_node->head = x;
    new_node->tail = NULL;

    cgc_assign(&new_node->tail, *list);
    cgc_assign(list, new_node);

    cgc_scope_end();
}

int main()
{
    cgc_scope_start();

    cgc_scope_declare(list_t *, list);
    cgc_alloc(&list, list_t);

    list->head = 0;
    list->tail = NULL;

    push(&list, 1);
    push(&list, 2);
   
    cgc_collect();

    cgc_scope_end();

    cgc_collect();

    return 0;
}
