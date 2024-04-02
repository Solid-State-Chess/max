#pragma once

#define MAX_BOARD_MAX_PLY (300)
/// Maximum number of pieces that may be on a chessboard
#define MAX_BOARD_PIECES_COUNT (32)

/// Length of a one-dimensional array representing a chessboard
#define MAX_BOARD_0x88_LEN (128)

#ifdef __GNUC__

#define MAX_INLINE_ALWAYS inline __attribute__((always_inline))

#define MAX_HOT __attribute__((hot))

#elif defined(_MSC_VER)

#define MAX_INLINE_ALWAYS inline __forceinline
#define MAX_HOT

#else

#define MAX_INLINE_ALWAYS inline
#define MAX_HOT

#endif
