#pragma once
#include <stdint.h>
#include <stddef.h>
#include "max/def.h"
#include "square.h"

#define MAX_TOTAL_PAWNS (8)
#define MAX_TOTAL_KNIGHTS (10)
#define MAX_TOTAL_BISHOPS (10)
#define MAX_TOTAL_ROOKS (10)
#define MAX_TOTAL_QUEENS (9)

/// Index into a #max_piecelist_t array.
/// Stored in the #max_pieces_t::index array mapping board positions to list indices
/// in a #max_pieces_t structure.
typedef uint8_t max_lidx_t;

/// A variable-sized piece array with length and board positions of one type of piece.
/// This structure provides a uniform interface for piece lists of static size contained in
/// #max_pieces_t.
typedef struct {
    /// Length of the #pos list
    max_lidx_t len;
    /// Variable-sized positions array
    max_bpos_t pos[];
} max_piecelist_t;

/// Create a new unnamed struct type that can be aliased by a pointer to #max_piecelist_t
#define PIECELIST(size) struct { max_lidx_t len; max_bpos_t pos[(size)]; }

/// A complete list of the positions of the pieces on one side of the board
typedef struct {
    PIECELIST(MAX_TOTAL_PAWNS) pawns;
    PIECELIST(MAX_TOTAL_KNIGHTS) knights;
    PIECELIST(MAX_TOTAL_BISHOPS) bishops;
    PIECELIST(MAX_TOTAL_ROOKS) rooks;
    PIECELIST(MAX_TOTAL_QUEENS) queens;
    /// King position stored as a piece list with length one in order to unify 
    /// list handling
    PIECELIST(1) king;
    /// 0x88 map of #max_bpos_t to #max_lidx_t in the list specified by the piece type that is on the same square
    max_lidx_t index[MAX_BOARD_0x88_LEN];
} max_pieces_t;

#undef PIECELIST

/// A piece code with color, validity, and type specifiers
///
/// \section layout Layout
/// []
typedef uint8_t max_piececode_t;

enum {
    MAX_PIECECODE_WHITE = 0x20,
    MAX_PIECECODE_BLACK = 0x40,
    /// Number of bits from the LSB that the black color flag is at (used to convert piececode -> turn bool)
    MAX_PIECECODE_BLACK_OFFSET = 6,
    MAX_PIECECODE_WHITE_OFFSET = 5,
    MAX_PIECECODE_INVAL = 0x80,
    MAX_PIECECODE_EMPTY = 0x00,

    MAX_PIECECODE_PAWN   = 0x01,
    MAX_PIECECODE_KNIGHT = 0x02,
    MAX_PIECECODE_BISHOP = 0x04,
    MAX_PIECECODE_ROOK   = 0x08,
    MAX_PIECECODE_QUEEN  = MAX_PIECECODE_BISHOP | MAX_PIECECODE_ROOK,
    MAX_PIECECODE_KING   = 0x10,
    
    MAX_PIECECODE_DIAGONAL_MASK = MAX_PIECECODE_BISHOP,
    MAX_PIECECODE_CARDINAL_MASK = MAX_PIECECODE_ROOK,

    ///
    MAX_PIECECODE_TYPE_MASK   = 0x1F,
    MAX_PIECECODE_COLOR_MASK  = 0x60,
};

/// \ingroup utils
/// Get the positions list for the given piece code.
/// Internally, this looks up an address directly using the #max_piececode_t & #MAX_PIECECODE_TYPE_MASK -
/// so invalid piece types or empty piece codes will return an undefined result.
///
/// \return A pointer to the #max_piecelist_t inside of #pieces that stores positions for pieces of the same kind
/// as #piece
MAX_INLINE_ALWAYS max_piecelist_t* max_pieces_get_list(max_pieces_t *pieces, max_piececode_t piece) {
    static const uint8_t lookup[17] = {
        [MAX_PIECECODE_PAWN]   = offsetof(max_pieces_t, pawns),
        [MAX_PIECECODE_KNIGHT] = offsetof(max_pieces_t, knights),
        [MAX_PIECECODE_BISHOP] = offsetof(max_pieces_t, bishops),
        [MAX_PIECECODE_ROOK]   = offsetof(max_pieces_t, rooks),
        [MAX_PIECECODE_QUEEN]  = offsetof(max_pieces_t, queens),
        [MAX_PIECECODE_KING]   = offsetof(max_pieces_t, king)
    };

    return (max_piecelist_t*)((uint8_t*)pieces + lookup[piece & MAX_PIECECODE_TYPE_MASK]);
}

/// \ingroup utils
/// Get a piececode bitmask for piece types that can attack along the given direction
/// (returns bishop for diagonals and rook for cardinals)
MAX_INLINE_ALWAYS max_piececode_t max_get_piece_mask_attacks_direction(max_increment_t dir) {
    if(max_increment_is_diagonal(dir)) {
        return MAX_PIECECODE_DIAGONAL_MASK;
    } else {
        return MAX_PIECECODE_CARDINAL_MASK;
    }
}
