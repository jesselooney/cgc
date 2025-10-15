#ifndef ARC_H
#define ARC_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

// ==============================================
// Signatures
// ==============================================

void arc_alloc(void **p, size_t size,
               void (*map_ptrs)(void *, void (*f)(void *)));

void arc_assign(void **p, void *q);

void arc_register(int arg_count, ...);

void arc_delete(void **p);

// ==============================================
// Definitions
// ==============================================

typedef struct {
    size_t ref_count;
    void (*map_ptrs)(void *, void(*f)(void *));
} arc_header_t;

static arc_header_t *_arc_get_header_ptr(void *p)
{
    return (arc_header_t *) p - 1;
}

static void _arc_inc(void *p)
{
    _arc_get_header_ptr(p)->ref_count++;
}

static void _arc_dec(void *p)
{
    arc_header_t *header = _arc_get_header_ptr(p);
    header->ref_count--;
    if (header->ref_count <= 0) {
        if (header->map_ptrs != NULL)
            (*header->map_ptrs) (p, arc_delete);
        free(header);
    }
}

static bool _arc_is_heap_ptr(void *p)
{
    // TODO: Implement this.
    return p != NULL;
}

void arc_alloc(void **p, size_t size,
               void (*map_ptrs)(void *, void (*f)(void *)))
{
    arc_header_t *header = malloc(size + sizeof(arc_header_t));

    header->ref_count = 1;
    header->map_ptrs = map_ptrs;

    *p = (void *) (header + 1);
}

void arc_assign(void **p, void *q)
{
    // Increment before decrementing because decrementing is potentially
    // destructive: arc_assign(&p, p) could fail if the order were reversed.
    if (_arc_is_heap_ptr(q))
        _arc_inc(q);
    if (_arc_is_heap_ptr(*p))
        _arc_dec(*p);

    *p = q;
}

void arc_register(int arg_count, ...)
{
    va_list args;
    va_start(args, arg_count);

    for (int i = 0; i < arg_count; i++) {
        void *p = va_arg(args, void *);
        if (_arc_is_heap_ptr(p))
            _arc_inc(p);
    }

    va_end(args);
}

void arc_delete(void **p)
{
    if (_arc_is_heap_ptr(*p))
        _arc_dec(*p);

    *p = NULL;
}

#endif
