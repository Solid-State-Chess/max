#pragma once

#ifdef MAX_ASSERTS
#include <assert.h>
#define MAX_ASSERT(...) assert(__VA_ARGS__)
#define MAX_ASSERT_WITH(cond, ...) \
    do {                           \
        if(!(cond)) {              \
            __VA_ARGS__            \
        }                          \
        assert(cond);              \
    } while(0)
#else
#define MAX_ASSERT(...)
#define MAX_ASSERT_WITH(...)
#endif

#ifdef MAX_ASSERTS_SANITY
#include <assert.h>
#define MAX_SANITY(...) assert(__VA_ARGS__)
#define MAX_SANITY_WITH(cond, ...) \
    do {                           \
        if(!(cond)) {              \
            __VA_ARGS__            \
        }                          \
        assert(cond);              \
    } while(0)
#else
#define MAX_SANITY(...)
#define MAX_SANITY_WITH(...)
#endif
