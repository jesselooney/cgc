#ifndef STACK_H
#define STACK_H


typedef struct {
    void** arr[1000];
    int    top;
} stack_t;

///////////////

stack_t* stack_init();
void stack_push(stack_t* s, void** p);
void** stack_pop(stack_t* s);

/////////////////

stack_t* stack_init() {
    stack_t* s = malloc(sizeof(stack_t));
    s->top = -1;
    return s;
}

void stack_push(stack_t* s, void** p) {
    s->arr[++s->top] = p;
}

void** stack_pop(stack_t* s) {
    return s->arr[s->top--];
}




#endif
