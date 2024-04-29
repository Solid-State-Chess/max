/// \file piececode.h

#pragma once

#include <stdint.h>

/// \ingroup board
/// @{

/// \defgroup piececode Piece Codes
/// Piece codes form one half of the actual chessboard - storing piece type and color by square.
/// This allows efficient lookups during move generation and check detection.
/// @{

/// A single-byte piece code containing color and piece kind data.
/// Specific values were selected for the piececode type values -
/// bishops and rook tags share a bit with the queen tag, allowing
/// efficient is-slider check with a single bitwise operation.
/// @{
typedef uint8_t max_piececode_t;


enum {
    MAX_PIECECODE_PAWN = 0x02,
};

/// @}

/// @}

/// @}
