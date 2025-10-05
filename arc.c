#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "arc.h"

typedef struct {
    size_t ref_count;
    void (*destructor)(void *);
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
        if (header->destructor != NULL)
            (*header->destructor) (p);
        free(header);
    }
}

static bool arc_is_heap_ptr(void *p)
{
    // TODO: Implement this.
    return p != NULL;
}

void arc_alloc(void **p, size_t size)
{
    arc_header_t *header = malloc(size + sizeof(arc_header_t));

    header->ref_count = 1;
    header->destructor = NULL;

    *p = (void*) (header + 1);
}

void arc_set_destructor(void *p, void (*destructor)(void *))
{
    if(arc_is_heap_ptr(p))
        arc_get_header_ptr(p)->destructor = destructor;
}

void arc_create(void *p)
{
    if (arc_is_heap_ptr(p))
        arc_inc(p);
}

void arc_delete(void **p)
{
    if (arc_is_heap_ptr(*p))
        arc_dec(*p);

    *p = NULL;
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
