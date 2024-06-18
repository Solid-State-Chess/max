/// \file def.h

#pragma once

/// On supported platforms, enable forced inlining of a given function
/// @{
#ifdef __GNUC__

#define MAX_INLINE_ALWAYS __attribute__((always_inline)) inline

#else

#define MAX_INLINE_ALWAYS inline

#endif

/// @}
