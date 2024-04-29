/// \file def.h

#pragma once

#ifdef __GNUC__

#define MAX_INLINE_ALWAYS __attribute__((always_inline)) inline

#else

#define MAX_INLMAX_INLINE_ALWAYS inline

#endif
