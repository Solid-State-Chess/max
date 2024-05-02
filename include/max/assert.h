#pragma once

#ifdef MAX_ASSERTS
#include <assert.h>
#define MAX_ASSERT(...) assert(__VA_ARGS__)
#else
#define MAX_ASSERT(...)
#endif

#ifdef MAX_ASSERTS_SANITY
#include <assert.h>
#define MAX_SANITY(...) assert(__VA_ARGS__)
#else
#define MAX_SANITY(...)
#endif
