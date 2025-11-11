#ifndef ARC_H
#define ARC_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "debug.h"

// ==============================================
// Members
// ==============================================

typedef struct {
    size_t ref_count;
    void (*map_ptrs)(void *, void(*f)(void *));
} _arc_header_t;

void arc_alloc(void **p, size_t size,
               void (*map_ptrs)(void *, void(*f)(void *)));
void arc_assign(void **p, void *q);
void arc_register(void *p);
void arc_deregister(void *p);

static _arc_header_t *_arc_get_header_ptr(void *p);
static bool _arc_is_heap_ptr(void *p);
static void _arc_inc(void *p);
static void _arc_dec(void *p);

// ==============================================
// Definitions
// ==============================================

void arc_alloc(void **p, size_t size,
               void (*map_ptrs)(void *, void(*f)(void *)))
{
    _arc_header_t *header = malloc(size + sizeof(_arc_header_t));

    header->ref_count = 1;
    header->map_ptrs = map_ptrs;

    *p = (void *) (header + 1);
    log_trace("a %p", *p);
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

void arc_register(void *p)
{
    log_info("arc_register(%p)", p);
    if (_arc_is_heap_ptr(p))
        _arc_inc(p);
}

void arc_deregister(void *p)
{
    log_info("arc_deregister(%p)", p);
    if (_arc_is_heap_ptr(p))
        _arc_dec(p);
}

static _arc_header_t *_arc_get_header_ptr(void *p)
{
    return (_arc_header_t *) p - 1;
}

static bool _arc_is_heap_ptr(void *p)
{
    // TODO: Implement this.
    return p != NULL;
}

static void _arc_inc(void *p)
{
    log_info("_arc_inc(%p)", p);
    _arc_header_t *header = _arc_get_header_ptr(p);
    header->ref_count++;
    log_debug("ref_count++ == %ld", header->ref_count);

}

static void _arc_dec(void *p)
{
    log_info("_arc_dec(%p)", p);
    _arc_header_t *header = _arc_get_header_ptr(p);
    header->ref_count--;
    log_debug("ref_count-- == %ld", header->ref_count);

    if (header->ref_count <= 0) {
        if (header->map_ptrs != NULL)
            (*header->map_ptrs) (p, arc_deregister);
        log_trace("f %p", p);
        free(header);
    }
}

#endif
