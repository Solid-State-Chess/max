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

/// \name Piece Type Flags
/// @{

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
    /// Mask for the lower four bits of a #max_piececode_t, giving the piece type alone when masked
    MAX_PIECECODE_TYPE_MASK   = 0x0F,
};


/// @}


/// \name Piece Type Indexes
/// While the piece type flags in #max_piececode_t are selected for efficient bitwise checks,
/// they waste space when directly used as lookup table indices.
/// Functions are provided to map from piece types to their dense indexes
/// @{

/// A dense index guaranteed to be in the range 0..5 representing all six unique
/// chess piece types
typedef uint8_t max_pieceindex_t;

enum {
    MAX_PIECEINDEX_PAWN   = 0,
    MAX_PIECEINDEX_KNIGHT = 1,
    MAX_PIECEINDEX_BISHOP = 2,
    MAX_PIECEINDEX_ROOK   = 3,
    MAX_PIECEINDEX_QUEEN  = 4,
    MAX_PIECEINDEX_KING   = 5,
    /// The required length of an array when indexed by a #max_pieceindex_t
    MAX_PIECEINDEX_LEN    = 6,
};

/// Get an index in the range 0..5 to represent the kind of chess piece this in as a continuous index.
/// The values chosen for #max_piececode_t type flags are optimized for quick is-sliding checks, but when used as an index
/// we waste some space (10 elements of a lookup table).
/// In order to provide the best of both worlds (for example in zobrist element lookup tables), this function efficiently maps
/// a piece type to a unique index.
max_pieceindex_t max_piececode_kind_index(max_piececode_t piece);

/// @}

/// @}


/// @}

/// @}
