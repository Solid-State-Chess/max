/// \file side.h

#pragma once
#include <stdint.h>

/// \ingroup board
/// @{

/// Simple typedef over a single byte for an index used to identify one of the players.
/// During nearly all phases of engine search, we use this index to grab side-specific
/// data from an array with two elements - avoiding a branch.
///
/// White = 0
///
/// Black = 1
///
///
/// These specific numbers are chosen because we can very efficiently map a ply counter to 
/// a side index by bitwise ANDing with 1.
typedef uint8_t max_side_t;

/// @}