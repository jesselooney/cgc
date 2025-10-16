#ifndef MACROS_H
#define MACROS_H

// defined for void*
#define ARG_COUNT(...) (sizeof((void*[]) {__VA_ARGS__}) / sizeof(void*))

#endif
