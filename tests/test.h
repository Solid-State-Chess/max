#pragma once
#include <stdio.h>

#define ASSERT(expr, ...) do {          \
    if(!(expr)) {                       \
        fputs(#expr, stderr);           \
        fputc('\n', stderr);            \
        fprintf(stderr, __VA_ARGS__);   \
        return 1;                       \
    }                                   \
} while(0)
