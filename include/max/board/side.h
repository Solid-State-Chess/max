/// \file side.h

#pragma once
#include <stdint.h>

/// \ingroup board
/// @{

/// Simple typedef over a single byte for an index used to identify one of the players in arrays.
/// White is represented as 0, while black is 1.
/// These specific numbers are chosen because we can very efficiently map a ply counter to 
/// a side index by bitwise ANDing with 1.
typedef uint8_t max_side_t;

/// @}
