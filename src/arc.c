#include <stdbool.h>

#include <stdint.h>
#include <stdlib.h>

#include <stdarg.h>

#include "arc.h"

typedef struct {
    size_t ref_count;
    void (*map_ptrs)(void*, void (*f)(void*));
} arc_header_t;

static arc_header_t *arc_get_header_ptr(void *p)
{
    return (arc_header_t *) p - 1;
}

static void arc_inc(void *p)
{
    arc_get_header_ptr(p)->ref_count++;
}

static void arc_dec(void *p)
{
    arc_header_t *header = arc_get_header_ptr(p);
    header->ref_count--;
    if (header->ref_count <= 0) {
        if (header->map_ptrs != NULL)
            (*header->map_ptrs)(p, arc_delete);
        free(header);
    }
}

static bool arc_is_heap_ptr(void *p)
{
    // TODO: Implement this.
    return p != NULL;
}

// ========
//  PUBLIC
// ========

void arc_alloc(void **p, size_t size)
{
    arc_header_t *header = malloc(size + sizeof(arc_header_t));

    header->ref_count = 1;
    header->map_ptrs = NULL;

    *p = (void*) (header + 1);
}

void arc_assign(void **p, void *q)
{
    // Increment before decrementing because decrementing is potentially
    // destructive: arc_assign(&p, p) could fail if the order were reversed.
    if (arc_is_heap_ptr(q))
        arc_inc(q);
    if (arc_is_heap_ptr(*p))
        arc_dec(*p);

    *p = q;
}

//void arc_create(void *p)
//{
//    if (arc_is_heap_ptr(p))
//        arc_inc(p);
//}

void arc_register(int arg_count, ...) {
    va_list args;
    va_start(args, arg_count);

    for (int i = 0; i < arg_count; i++) {
        void *p = va_arg(args, void*);
        if (arc_is_heap_ptr(p))
            arc_inc(p);
    }

    va_end(args);
}

void arc_delete(void **p)
{
    if (arc_is_heap_ptr(*p))
        arc_dec(*p);

    *p = NULL;
}

void arc_set_map_ptrs(
    void *p,
    void (*map_ptrs)(void*, void (*f)(void*))
)
{
    if (arc_is_heap_ptr(p))
        arc_get_header_ptr(p)->map_ptrs = map_ptrs;
}

