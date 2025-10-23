#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// ==============================================
// Members
// ==============================================

// #define log_error
// #define log_warn
// #define log_trace
// #define log_info
// #define log_debug

// #define _log_x_inner

// ==============================================
// Members
// ==============================================

#ifndef VERBOSITY
#define VERBOSITY 2
#endif

#if VERBOSITY >= 1
#define log_error(MESSAGE, ...) _log_x_inner("[DEBUG]", MESSAGE, ##__VA_ARGS__)
#else
#define log_error
#endif

#if VERBOSITY >= 2
#define log_warn(MESSAGE, ...)  _log_x_inner("[WARN ]", MESSAGE, ##__VA_ARGS__)
#else
#define log_warn
#endif

/// TRACE should be used to provide the minimal logging of execution flow
/// necessary to verify the correctness of the program. Calls to TRACE should
/// be considered stable API, because they will be checked in golden tests.
#if VERBOSITY >= 3
#define log_trace(MESSAGE, ...) fprintf(stdout, "[TRACE] " MESSAGE "\n", ##__VA_ARGS__);
#else
#define log_trace
#endif

#if VERBOSITY >= 4
#define log_info(MESSAGE, ...)  _log_x_inner("[INFO ]", MESSAGE, ##__VA_ARGS__)
#else
#define log_info
#endif

#if VERBOSITY >= 5
#define log_debug(MESSAGE, ...) _log_x_inner("[DEBUG]", MESSAGE, ##__VA_ARGS__)
#else
#define log_debug
#endif

#define _log_x_inner(LABEL, MESSAGE, ...) fprintf(stdout, LABEL " (%s:%d) " MESSAGE "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
