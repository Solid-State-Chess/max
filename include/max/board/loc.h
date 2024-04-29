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

/// @}


/// @}
