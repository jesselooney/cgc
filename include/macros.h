#ifndef MACROS_H
#define MACROS_H

// defined for void*
#define ARG_COUNT(...) (sizeof((void*[]) {__VA_ARGS__}) / sizeof(void*))

// Hack to test if T is a pointer type by checking if the last character of the
// type name is '*'.
// WARN: Does not account for the possibility of typedefs.
#define IS_PTR_TYPE(T) (#T[sizeof(#T)-2] == '*')

#endif
