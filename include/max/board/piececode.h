/// \file piececode.h

#pragma once

#include "max/assert.h"
#include "max/board/side.h"
#include "max/def.h"
#include <stdbool.h>
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
    /// Code representing an invalid (off the board for 0x88 indexing) square
    MAX_PIECECODE_INVALID = 0x80,
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

/// \name Piece Color Flags
/// @{

enum {
    /// #max_piececode_t flag representing that the given piece is white
    MAX_PIECECODE_WHITE = 0x20,
    /// #max_piececode_t flag representing that the given piece is black
    MAX_PIECECODE_BLACK = 0x40,
    /// Number of bits from the LSB that the black color flag is at (used to convert piececode -> turn bool)
    MAX_PIECECODE_BLACK_OFFSET = 6,
    /// Offset in bits from the LSB of the #MAX_PIECECODE_WHITE bitmask
    MAX_PIECECODE_WHITE_OFFSET = 5,
    /// Bitmask for the two color bits of a #max_piececode_t
    MAX_PIECECODE_COLOR_MASK = 0x60,
};

/// @}

/// Helper function to create a piececode from a side and type tag
MAX_INLINE_ALWAYS max_piececode_t max_piececode_new(uint8_t side, uint8_t type) {
    return (max_piececode_t){.v = side | type };
}

/// Get a side mask to use in either #max_piecemask_t or in #max_piececode_new representing the color
/// bits that will be set for pieces of the given side.
MAX_INLINE_ALWAYS uint8_t max_piececode_color_for_side(max_side_t side) {
    return MAX_PIECECODE_WHITE << side;
}

/// Get a #max_side_t representing the color of the given piece.
/// The piece should not be empty or invalid, as without debug assertions these
/// pieces will be represented as white.
/// \return 1 if the given piece is black, and 0 if it is white or an empty piece
MAX_INLINE_ALWAYS max_side_t max_piececode_side(max_piececode_t piece) {
    MAX_ASSERT(piece.v != MAX_PIECECODE_EMPTY);
    return (piece.v >> MAX_PIECECODE_BLACK_OFFSET) & 1;
}


/// \name Piece Bitmasks
/// @{

/// Wrapper struct for a bitmask that can mask a #max_piececode_t.
/// The type is separate from the piece code type to reduce the likelihood of
/// errors when combining piece codes
typedef struct { uint8_t msk; } max_piecemask_t;

#define MAX_PIECEMASK_EMPTY    ((max_piecemask_t){ .msk = 0 })

#define MAX_PIECEMASK_DIAGONAL ((max_piecemask_t){ .msk = MAX_PIECECODE_BISHOP })

#define MAX_PIECEMASK_CARDINAL ((max_piecemask_t){ .msk = MAX_PIECECODE_ROOK   })

/// Combine the two given piece code masks into a single mask that can be tested in one operation.
MAX_INLINE_ALWAYS max_piecemask_t max_piecemask_combine(max_piecemask_t v1, max_piecemask_t v2) {
    return (max_piecemask_t){ .msk = v1.msk | v2.msk };
}

/// Get a new #max_piecemask_t wrapper struct from the given raw bitmask.
MAX_INLINE_ALWAYS max_piecemask_t max_piecemask_new(uint8_t mask) {
    return (max_piecemask_t){ .msk = mask };
}

/// Check if the given piece matches the bitmask
/// \param Piece the piece bitfield to mask with a bitwise AND
/// \param mask Bitmask to apply to the given piece
MAX_INLINE_ALWAYS bool max_piececode_match(max_piececode_t piece, max_piecemask_t mask) {
    return (piece.v & mask.msk) != 0;
}

/// Get a bitmask that matches piece codes of the same color as the given piece.
/// \param piece The piece to extract the color bits from, must not be an invalid or empty piece code
MAX_INLINE_ALWAYS max_piecemask_t max_piececode_color_mask(max_piececode_t piece) {
    return (max_piecemask_t){ .msk = piece.v & MAX_PIECECODE_COLOR_MASK };
}

/// Get a piece bitmask that matches pieces of the given side
MAX_INLINE_ALWAYS max_piecemask_t max_side_color_mask(max_side_t side) {
    return (max_piecemask_t){ .msk = MAX_PIECECODE_WHITE << side };
}

/// Get a piece bitmask that matches enemy pieces of the given side
MAX_INLINE_ALWAYS max_piecemask_t max_side_enemy_color_mask(max_side_t side) {
    return (max_piecemask_t){ .msk = MAX_PIECECODE_BLACK >> side };
}

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
