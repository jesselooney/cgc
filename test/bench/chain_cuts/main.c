#include "cgc.h"

typedef struct list {
    int head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t *a, void (*f)(void *))
{
    (*f) (a->tail);
}

void push(list_t **list_handle, int value)
{
    cgc_scope_start(&list_handle);

    cgc_scope_declare(list_t *, old_head);
    cgc_assign(&old_head, *list_handle);
   
    cgc_scope_declare(list_t *, new_head);
    cgc_alloc(&new_head, list_t);

    new_head->head = value;
    cgc_assign(&new_head->tail, old_head);

    cgc_assign(list_handle, new_head);

    cgc_scope_end();
}

void iterate(list_t **list_handle, int live_size, int dead_size_incr)
{
    cgc_scope_start(&list_handle);

    // The last link in the chain that will remain live after this iteration.
    cgc_scope_declare(list_t *, last_live);

    for (int i = 0; i < dead_size_incr + live_size; i++) {
        push(list_handle, 31);
        if (i == dead_size_incr) {
            // At this point, we have pushed `dead_size_incr + 1` new links onto
            // the chain, so if we keep the current link alive and nothing
            // beyond, we will have added the correct amount of dead links.
            cgc_assign(&last_live, *list_handle);
        }
    }

    // Lose the reference to the links in the chain beyond `last_live`.
    cgc_assign(&last_live->tail, NULL);

    cgc_scope_end();
}

int main()
{
    cgc_scope_start();

    int total_iterations  = 10;
    int target_recordings = 10;
    int threshold = total_iterations / target_recordings;

    cgc_scope_declare(list_t *, list);
    cgc_assign(&list, NULL);

    for (int i = 0; i < total_iterations; i++) {
        iterate(&list, 9999, 0);
        if (i % threshold == 0) {
            cgc_monitor_write_user("progress,%d\n", i);
        }
    }

    cgc_scope_end();
    return 0;
}
