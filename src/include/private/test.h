#pragma once

#ifdef MAX_TESTS
#include <stdio.h>
#include <stdint.h>

extern uint32_t _max_failed_tests;
extern uint32_t _max_tests;

#define MAX_TEST_ASSERT_WITH(expr, ...) do {                          \
    _max_tests += 1;                                                  \
    if(!(expr)) {                                                     \
        fprintf(stderr, "    [FAIL] - " __FILE__ ": %d\n", __LINE__); \
        fputs(#expr, stderr);                                         \
        fputc('\n', stderr);                                          \
        __VA_ARGS__                                                   \
        fputc('\n', stderr);                                          \
        _max_failed_tests += 1;                                       \
    }                                                                 \
} while(0)

#define ASSERT(expr, ...) MAX_TEST_ASSERT_WITH(expr, fprintf(stderr, __VA_ARGS__);)

#define CATEGORY(func, name) do {             \
    uint32_t before = _max_tests;             \
    uint32_t before_fail = _max_failed_tests; \
    (func)();                                 \
    uint32_t tests = _max_tests - before;     \
    printf("[" name "]  - [%d / %d]\n", tests - (_max_failed_tests - before_fail), tests); \
} while(0)

#endif
