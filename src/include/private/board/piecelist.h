#pragma once

#include <stddef.h>
#include "max/board/piececode.h"
#include "max/board/piecelist.h"
#include "max/def.h"

/// \ingroup piecelist
/// @{

/// \name Private Functions
/// @{

/// Initialize the lengths of all piece lists in the given collection to 0
MAX_INLINE_ALWAYS void max_pieces_new(max_pieces_t *pieces) {
    pieces->pawn.len = 0;
    pieces->knight.len = 0;
    pieces->bishop.len = 0;
    pieces->rook.len = 0;
    pieces->queen.len = 0;
    pieces->king.len = 0;
}

/// Get the board locations list for pieces of the given type
MAX_INLINE_ALWAYS max_loclist_t* max_pieces_get_list(max_pieces_t *pieces, max_piececode_t piece) {
    piece.v &= MAX_PIECECODE_TYPE_MASK;
    MAX_ASSERT(
        piece.v == MAX_PIECECODE_PAWN ||
        piece.v == MAX_PIECECODE_KNIGHT ||
        piece.v == MAX_PIECECODE_BISHOP ||
        piece.v == MAX_PIECECODE_ROOK ||
        piece.v == MAX_PIECECODE_QUEEN ||
        piece.v == MAX_PIECECODE_KING
    );
    static const uint8_t OFFSETS[16] = {
        [MAX_PIECECODE_PAWN]   = offsetof(max_pieces_t, pawn),
        [MAX_PIECECODE_KNIGHT] = offsetof(max_pieces_t, knight),
        [MAX_PIECECODE_BISHOP] = offsetof(max_pieces_t, bishop),
        [MAX_PIECECODE_ROOK]   = offsetof(max_pieces_t, rook),
        [MAX_PIECECODE_QUEEN]  = offsetof(max_pieces_t, queen),
        [MAX_PIECECODE_KING]   = offsetof(max_pieces_t, king)
    };

    return (max_loclist_t*)(((void*)pieces) + OFFSETS[piece.v & MAX_PIECECODE_TYPE_MASK]);
}

/// Add a board location to the given locations list.
/// No capacity checks are performed here, even with debug assertions enabled
/// (the capacity is not known alone, we must also know the piece type first)
/// \return Index of the added piece in the list
MAX_INLINE_ALWAYS max_lidx_t max_loclist_add(max_loclist_t *pieces, max_0x88_t pos) {
    pieces->loc[pieces->len] = pos;
    pieces->len += 1;
    return pieces->len - 1;
}

/// Remove the piece at the given index from the list
MAX_INLINE_ALWAYS void max_loclist_remove(max_loclist_t *pieces, max_lidx_t idx) {
    MAX_ASSERT(pieces->len > 0);
    pieces->len -= 1;
    pieces->loc[idx] = pieces->loc[pieces->len];
}


#ifdef MAX_TESTS

/// Perform unit tests for the #max_loclist_t and #max_pieces_t structures.
void max_pieces_unit_tests(void);

#endif

/// @}

/// @}
