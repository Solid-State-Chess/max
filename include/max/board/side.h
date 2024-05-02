/// \file side.h

#pragma once
#include <stdint.h>

/// \ingroup board
/// @{

/// Simple typedef over a single byte for an index used to identify one of the players.
/// During nearly all phases of engine search, we use this index to grab side-specific
/// data from an array with two elements - avoiding a branch.
///
/// ```
/// White = 0
/// Black = 1
/// ```
/// These specific numbers are chosen because we can very efficiently map a ply counter to 
/// a side index by bitwise ANDing with 1.
typedef uint8_t max_side_t;

enum {
    /// Definition for white side as stored in a #max_side_t
    MAX_SIDE_WHITE = 0,
    /// Index into arrays for the black side as stored in a #max_side_t
    MAX_SIDE_BLACK = 1,
};

/// Required size of a lookup table when indexed by a #max_side_t
#define MAX_SIDES_LEN (2)

/// @}
