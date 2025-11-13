// ==============================================
// Parameters
// ==============================================

#define OBJ_COUNT_EXP 10
#define ASSIGN_COUNT_EXP 28

// ==============================================
// Implementation
// ==============================================

#include "cgc.h"

typedef struct list {
    uint8_t head;
    struct list *tail;
} list_t;

void list_t__map_ptrs(list_t *a, void (*f)(void *))
{
    (*f) (a->tail);
}

#define OBJ_COUNT (1 << OBJ_COUNT_EXP)
#define RAND_MASK (~(~0 << OBJ_COUNT_EXP))
#define ASSIGN_COUNT (1 << ASSIGN_COUNT_EXP)

int main() {
    cgc_init();
    cgc_scope_start();

    srand(0);

    list_t **objs = malloc(OBJ_COUNT * sizeof(list_t *));

    // Allocate `OBJ_COUNT` lists.
    for (int i = 0; i < OBJ_COUNT; i++) {
        cgc_alloc(&objs[i], list_t);
    }

    // Perform `ASSIGN_COUNT` pointer assignments.
    for (int assign_num = 0; assign_num < ASSIGN_COUNT; assign_num++) {
        int i = rand() & RAND_MASK; 
        int j = rand() & RAND_MASK;
        cgc_assign(&objs[i]->tail, &objs[j]);
    }

    cgc_scope_end();
    return 0;
}
