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

    cgc_scope_declare(list_t **, ptr);

    cgc_scope_start();

    cgc_scope_declare(list_t **, arr);
    cgc_array(&arr, list_t *, 42);

    for (int i = 0; i < 42; i++) {
        arr[i] = NULL;
    }

    cgc_scope_declare(list_t *, list);
    cgc_alloc(&list, list_t);
    list->head = 0;
    list->tail = NULL;

    cgc_assign(&arr[31], list);

    // `ptr` is a pointer _inside_ of `arr`.
    cgc_assign(&ptr, &arr[13]);

    // Drop `arr` and `list`.
    cgc_scope_end();
    cgc_collect();
    
    // The objects should be kept alive here by `ptr`.
    log_trace("allocated objects should not have been collected yet");
    
    // Drop `ptr`, making the objects garbage.
    cgc_scope_end();
    cgc_collect();
    cgc_free(list);
    cgc_free(arr);

    log_trace("allocated objects should have been collected by now");

    return 0;
}
