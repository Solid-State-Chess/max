/// \file piececode.h

#pragma once

#include <stdint.h>

/// \ingroup board
/// @{

/// \defgroup piececode Piece Codes
/// Piece codes form one half of the actual chessboard - storing piece type and color by square.
/// This allows efficient lookups during move generation and check detection.
/// @{

/// A single-byte piece code. 
/// @{
typedef uint8_t max_piececode_t;


enum {
    MAX_PIECECODE_PAWN = 0x02,
};

/// @}

/// @}

/// @}
