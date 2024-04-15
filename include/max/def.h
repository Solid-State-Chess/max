#pragma once
/// \file
/// Definitions for compiler attributes and universal constants

/// Maximum number of pieces that may be on a chessboard.
/// Used for static size of capture stack in #max_board_t
#define MAX_BOARD_PIECES_COUNT (32)

/// Length of a one-dimensional array representing a chessboard when using the 
/// 0x88 coordinate scheme.
///
/// \see #max_bpos_t
#define MAX_BOARD_0x88_LEN (128)

#ifdef __GNUC__

/// Convenience macro that provides additional inlining hints to the compiler if supported
#define MAX_INLINE_ALWAYS inline __attribute__((always_inline))

/// Convenience macro providing additional optimization hints to the compiler
#define MAX_HOT __attribute__((hot))

#elif defined(_MSC_VER)

#define MAX_INLINE_ALWAYS inline __forceinline
#define MAX_HOT

#else

#define MAX_INLINE_ALWAYS inline
#define MAX_HOT

#endif
