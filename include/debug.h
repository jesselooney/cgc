#ifndef DEBUG_H
#define DEBUG_H

#ifndef DBG
#define DBG 0
#endif

#define debug(LEVEL, MESSAGE, ...) _debug_##LEVEL(MESSAGE, ##__VA_ARGS__)

#define _debug_inner(LEVEL, MESSAGE, ...) fprintf(stderr, "[DEBUG %d] (%s:%d) " MESSAGE "\n", LEVEL, __FILE__, __LINE__, ##__VA_ARGS__)

#if DBG >= 1
#define _debug_1(MESSAGE, ...) _debug_inner(1, MESSAGE, ##__VA_ARGS__)
#else
#define _debug_1
#endif

#if DBG >= 2
#define _debug_2(MESSAGE, ...) _debug_inner(2, MESSAGE, ##__VA_ARGS__)
#else
#define _debug_2
#endif

#if DBG >= 3
#define _debug_3(MESSAGE, ...) _debug_inner(3, MESSAGE, ##__VA_ARGS__)
#else
#define _debug_3
#endif

#endif
