#pragma once

#ifdef __GNUC__

#define MAX_INLINE_ALWAYS inline __attribute__((always_inline))

#elif defined(_MSC_VER)

#define MAX_INLINE_ALWAYS inline __forceinline

#else

#define MAX_INLINE_ALWAYS inline

#endif
