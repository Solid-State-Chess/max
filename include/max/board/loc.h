/// \file loc.h

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "max/def.h"

/// \ingroup board
/// @{

/// \defgroup loc Board Location Indices
/// Rank and file data stuffed into one byte using both the 0x88 and 6 bit LSF encoding schemes.
/// 0x88 is primarily used by the board for its index difference properties, but directly indexing arrays
/// using this encoding wastes 64 bytes of space, so for lookup table flexibility both encoding schemes are defined.
///
/// @{



/// \name 0x88 Indices
/// @{

/// A board rank and file location packed into a single byte using the 0x88 encoding scheme.
/// While less space efficient than 6 bit indices, 0x88 is more efficient in checking if a square is on the board or not,
/// and differences between 0x88 indices are unique with respect to distance and direction - allowing us to very efficiently map
/// the difference between two squares to a direction when checking for sliding attacks or pinned pieces.
/// \see x88diff
/// \see x88dir
typedef struct { uint8_t v; } max_0x88_t;

/// Length of an array when indexed using the 0x88 scheme.
#define MAX_0x88_LEN (128)

/// Bitmask for the lower 4 bits of a 0x88 position
#define MAX_0x88_FILE_MASK (0x0F)

/// Bitmask for the upper 4 bits of a 0x88 board position
#define MAX_0x88_RANK_MASK (0xF0)

/// Bit position of the 4 rank bits stored in a 0x88 index
#define MAX_0x88_RANK_POS  (4)

/// The bitmask that allows 0x88 positions to be checked for validity efficiently,
/// and where the encoding scheme gets its name.
/// \see max_0x88_valid()
#define MAX_0x88_INVALID_MASK (0x88)

/// Create a new 0x88 board index from the given rank and file coordinates.
/// \param rank Rank number, must be between 0 and 7 (not checked)
/// \param file File number, must be between 0 and 7
MAX_INLINE_ALWAYS max_0x88_t max_0x88_new(uint8_t rank, uint8_t file) {
    return (max_0x88_t){ .v = (rank << MAX_0x88_RANK_POS) | file };
}

/// Get the rank in the range [0..7] of the given 0x88 encoded position.
/// Note that for invalid positions, this may return out of bounds ranks.
MAX_INLINE_ALWAYS uint8_t max_0x88_rank(max_0x88_t pos) { return pos.v >> MAX_0x88_RANK_POS; }

/// Get the file index in the range [0..7] of the given 0x88 encoded position.
/// Like max_0x88_rank(), this may return out of bounds values if the position is invalid.
MAX_INLINE_ALWAYS uint8_t max_0x88_file(max_0x88_t pos) { return pos.v & MAX_0x88_FILE_MASK; }

/// Check if the 0x88 index represents a valid square on the chessboard
/// \return true if the position does not match the #MAX_0x88_INVALID_MASK bitmask
MAX_INLINE_ALWAYS bool max_0x88_valid(max_0x88_t pos) { return pos.v & MAX_0x88_INVALID_MASK; }

#ifdef MAX_CONSOLE

/// Utility macro to create two format arguments when using a printf "%c%c" string.
/// Formats board locations in algebraic a3, e4, etc. notation
#define MAX_0x88_FORMAT(loc) (max_0x88_file((loc).v) + 'a'), (max_0x88_rank((loc).v) + '1')

#endif



/// @}


/// \defgroup x88diff 0x88 Square Differences
/// The difference between two 0x88 board locations is unique with respect to distance and direction,
/// allowing us to map square differences to arrays very compactly.
/// This is most often used for check and pin detection, as it's very easy to determine if two squares lie
/// on the same diagonal or cardinal ray.
/// @{

/// 
typedef uint8_t max_0x88_diff_t;

/// @}

/// \name 6 Bit Indices
/// @{

/// Length of an array when indexed with the six-bit location encoding
#define MAX_6BIT_LEN (64)

/// A board rank and file location packed into 6 bits using Least Significant File encoding.
/// This representation is more compact than 0x88 and can only represent valid square indices between
/// 0 and 63, making it preferable for large lookup tables by square.
/// This indexing scheme is not used in the board itself because 0x88 proves more efficient in
/// off-the-board checks and square difference -> direction mapping.
/// \see max_0x88_to_6bit()
/// \see max_6bit_to_0x88()
typedef struct { uint8_t v; } max_6bit_t;

/// Bitmask for the lower 3 file bits of a #max_6bit_t.
#define MAX_6BIT_FILE_MASK (0x07)
/// Bitmask for the upper 3 rank bits of a #max_6bit_t
#define MAX_6BIT_RANK_MASK (0x38)
/// Bit position of the 3 rank bits stored in a #max_6bit_t
#define MAX_6BIT_RANK_POS (3)

/// Create a 6 bit wide board index using the given rank and file
/// \param rank The rank index of the created location, must be between 0 and 7
/// \param file The file index of the created location, must be between 0 and 7
MAX_INLINE_ALWAYS max_6bit_t max_6bit_new(uint8_t rank, uint8_t file) {
    return (max_6bit_t){ .v = (rank << MAX_6BIT_RANK_POS) | file };
}

/// Get the rank index between 0 and 7 of the given 6 bit board index.
MAX_INLINE_ALWAYS uint8_t max_6bit_rank(max_6bit_t pos) { return pos.v >> MAX_6BIT_RANK_POS; }
/// Get the file index between 0 and 7 of the given 6 bit board index.
MAX_INLINE_ALWAYS uint8_t max_6bit_file(max_6bit_t pos) { return pos.v & MAX_6BIT_FILE_MASK; }

/// @}

/// Pack a 0x88 board position into 6 bits from 0 to 63.
/// If the given position is not valid, the returned 6 bit index will also be invalid.
/// \see max_6bit_to_0x88()
MAX_INLINE_ALWAYS max_6bit_t max_0x88_to_6bit(max_0x88_t pos) {
    return (max_6bit_t){ .v = (pos.v + (pos.v & MAX_6BIT_FILE_MASK)) >> 1 };
}

/// Unpack a 6 bit board position to a full 0x88 position, guaranteed to be valid.
/// \see max_0x88_to_6bit()
MAX_INLINE_ALWAYS max_0x88_t max_6bit_to_0x88(max_6bit_t pos) {
    return (max_0x88_t){ .v = pos.v + (pos.v & ~MAX_6BIT_FILE_MASK) };
}


/// @}


/// @}
