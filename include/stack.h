#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <string.h>

#include "debug.h"

// ==============================================
// Members
// ==============================================

typedef struct {
    size_t max_size;
    size_t top;
    void ***items;
} stack_t;

stack_t *stack_init();
void stack_push(stack_t * s, void **p);
void **stack_pop(stack_t * s);
void stack_drop(stack_t * s);

static void _stack_grow(stack_t * s);

// ==============================================
// Definitions
// ==============================================

stack_t *stack_init(size_t initial_size)
{
    stack_t *s = malloc(sizeof(stack_t));

    s->max_size = initial_size;
    s->top = 0;
    s->items = malloc(sizeof(void **) * initial_size);

    return s;
}

void stack_push(stack_t *s, void **p)
{
    if (s->top == s->max_size)
        _stack_grow(s);
    s->items[s->top++] = p;
}

void **stack_pop(stack_t *s)
{
    if (s->top == 0) {
        log_error("Tried to stack_pop with top == 0");
        exit(1);
    }
    return s->items[--s->top];
}

void stack_drop(stack_t *s)
{
    free(s->items);
    free(s);
}

static void _stack_grow(stack_t *s)
{
    log_info("_stack_grow(%p)", s);
    size_t new_max_size = s->max_size * 2;
    void ***new_items = malloc(sizeof(void **) * new_max_size);

    memcpy(s->items, new_items, sizeof(void **) * s->top);
    free(s->items);

    s->max_size = new_max_size;
    s->items = new_items;
}

#endif
