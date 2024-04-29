/// \file loc.h

#pragma once

#include <stdint.h>

/// \ingroup board
/// @{

/// \defgroup loc Board Location Indices
/// Rank and file data stuffed into one byte using the 0x88 encoding scheme.
///
/// @{

/// Length of an array when indexed using the 0x88 scheme.
#define MAX_0x88_LEN (128)

/// A board rank and file location packed into a single byte using the 0x88 encoding scheme.
typedef uint8_t max_0x88_t;

/// Length of an array when indexed with the six-bit location encoding
#define MAX_6BIT_LEN (64)

/// A board rank and file location packed into 6 bits using Least Significant File encoding.
/// This representation is more compact than 0x88 and can only represent valid square indices between
/// 0 and 63, making it preferable for large lookup tables by square.
/// This indexing scheme is not used in the board itself because 0x88 proves more efficient in
/// off-the-board checks and square difference -> direction mapping.
typedef uint8_t max_6bit_t;

/// @}


/// @}
