#pragma once

#include <stdint.h>

extern uint32_t _max_failed_tests;
extern uint32_t _max_tests;

#define ASSERT(expr, ...) do {          \
    _max_tests += 1;                    \
    if(!(expr)) {                       \
        fprintf(stderr, "    [FAIL] - " __FILE__ ": %d\n", __LINE__); \
        fputs(#expr, stderr);           \
        fputc('\n', stderr);            \
        fprintf(stderr, __VA_ARGS__);   \
        fputc('\n', stderr);            \
        _max_failed_tests += 1;         \
    }                                   \
} while(0)

#define CATEGORY(func, name) do {             \
    uint32_t before = _max_tests;             \
    uint32_t before_fail = _max_failed_tests; \
    (func)();                                 \
    uint32_t tests = _max_tests - before;     \
    printf("[" name "]  - [%d / %d]\n", tests - (_max_failed_tests - before_fail), tests); \
} while(0)
