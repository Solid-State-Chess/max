/// \file loc.h

#pragma once

#include <stdint.h>
#include "max/def.h"

/// \ingroup board
/// @{

/// \defgroup loc Board Location Indices
/// Rank and file data stuffed into one byte using the 0x88 encoding scheme.
///
/// @{

/// Length of an array when indexed using the 0x88 scheme.
#define MAX_0x88_LEN (128)

/// A board rank and file location packed into a single byte using the 0x88 encoding scheme.
/// While less space efficient than 6 bit indices, 0x88 is more efficient in checking if a square is on the board or not,
/// and differences between 0x88 indices are unique with respect to distance and direction - allowing us to very efficiently map
/// the difference between two squares to a direction when checking for sliding attacks or pinned pieces.
typedef uint8_t max_0x88_t;

/// Length of an array when indexed with the six-bit location encoding
#define MAX_6BIT_LEN (64)

/// A board rank and file location packed into 6 bits using Least Significant File encoding.
/// This representation is more compact than 0x88 and can only represent valid square indices between
/// 0 and 63, making it preferable for large lookup tables by square.
/// This indexing scheme is not used in the board itself because 0x88 proves more efficient in
/// off-the-board checks and square difference -> direction mapping.
/// \see max_0x88_to_6bit()
/// \see max_6bit_to_0x88()
typedef uint8_t max_6bit_t;

/// Pack a 0x88 board position into 6 bits from 0 to 63.
/// If the given position is not valid, the returned 6 bit index will also be invalid.
/// \see max_6bit_to_0x88()
MAX_INLINE_ALWAYS max_6bit_t max_0x88_to_6bit(max_0x88_t pos) {
    return (pos + (pos & 7)) >> 1;
}

/// Unpack a 6 bit board position to a full 0x88 position, guaranteed to be valid.
/// \see max_0x88_to_6bit()
MAX_INLINE_ALWAYS max_0x88_t max_6bit_to_0x88(max_6bit_t pos) {
    return pos + (pos & ~7);
}

/// @}


/// @}
