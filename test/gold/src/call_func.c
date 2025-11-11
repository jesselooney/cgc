#include <assert.h>
#include <stddef.h>

#include "cgc.h"

typedef struct list {
    int head;
    struct list *tail;
} list_t;

static list_t static_list = { .head = 0, .tail = NULL };

void list_t__map_ptrs(list_t *a, void (*f)(void *))
{
    (*f) (a->tail);
}

void add_to_head(list_t *a, int x) {
    cgc_scope_start(&a);
    a->head += x;
    cgc_scope_end();
}

int main()
{
    cgc_init();
    cgc_scope_start();

    list_t stack_list = static_list;

    cgc_scope_declare(list_t *, heap_list);
    cgc_alloc(&heap_list, list_t);
    *heap_list = static_list;

    // `&static_list` is a statics pointer and should be ignored by the scope
    // code inside `add_to_head`.
    add_to_head(&static_list, 1);
    // `&stack_list` is a stack pointer and should be ignored by the scope code
    // inside `add_to_head`.
    add_to_head(&stack_list, 1);
    // `heap_list` is a heap pointer.
    add_to_head(heap_list, 1);

    assert(static_list.head == 1);
    assert(stack_list.head == 1);
    assert(heap_list->head == 1);

    cgc_scope_end();
    cgc_collect();
    return 0;
}
