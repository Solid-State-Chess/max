#pragma once
#include <stdio.h>

#define ASSERT_EQ(ty, e1, e2, fmt) do {                 \
    ty v = (e1);                                        \
    if((e1) != (e2)) {                                  \
        fputs(#e1 " != " #e2, stderr);                    \
        fputc('\n', stderr);                            \
        fprintf(stderr, #e1" = " fmt "\n", v);          \
    }                                                   \
} while(0)

#define ASSERT(expr, ...) do {          \
    if(!(expr)) {                       \
        fputs(#expr, stderr);           \
        fputc('\n', stderr);            \
        fprintf(stderr, __VA_ARGS__);   \
        return 1;                       \
    }                                   \
} while(0)

#define CATEGORY(func, name) do {           \
    fputs("TEST: " name "\n", stderr);      \
    int __result_##func = (func)();         \
    if(__result_##func != 0) {              \
        fputs(name " FAILED\n", stderr);    \
        return __result_##func;             \
    }                                       \
} while(0)


int board_tests(void);
int engine_tests(void);
