#include <stddef.h>

#define declare_stack(SELF, VAL) \
    typedef VAL SELF##_val; \
    typedef struct { SELF##_val *data, size_t size } SELF

