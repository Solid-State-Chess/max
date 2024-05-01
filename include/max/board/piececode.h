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
typedef struct { uint8_t v; } max_piececode_t;

enum {
    /// Code representing an empty square.
    MAX_PIECECODE_EMPTY  = 0x00,
    /// Code representing a pawn.
    MAX_PIECECODE_PAWN   = 0x01,
    /// Code representing a knight.
    MAX_PIECECODE_KNIGHT = 0x02,
    /// Code representing a king.
    /// The king code is not higher than the other codes in order to pack the 
    /// piece type into four bits as opposed to five.
    MAX_PIECECODE_KING   = 0x03,
    ///  Piece code identifying a bishop when identified alone, or identifying
    ///  any diagonally sliding piece when used as a bitmask.
    MAX_PIECECODE_BISHOP = 0x04,
    /// Piece code representing a rook, or any piece that can slide cardinally when
    /// used as a bitmask.
    MAX_PIECECODE_ROOK   = 0x08,
    /// Piece code uniquely representing a queen, containing both the bishop and
    /// rook bitmasks to allow efficient checks for sliding attackers by bitmask.
    MAX_PIECECODE_QUEEN  = MAX_PIECECODE_ROOK | MAX_PIECECODE_BISHOP,
};

/// @}


/// @}

/// @}
