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
MAX_INLINE_ALWAYS void max_plist_new(max_plist_t *pieces) {
    pieces->pawn.len = 0;
    pieces->knight.len = 0;
    pieces->bishop.len = 0;
    pieces->rook.len = 0;
    pieces->queen.len = 0;
    pieces->king.len = 0;
}

/// Get the board locations list for pieces of the given type
MAX_INLINE_ALWAYS max_loclist_t* max_plist_get_list(max_plist_t *pieces, max_piececode_t piece) {
    static const uint8_t OFFSETS[16] = {
        [MAX_PIECECODE_PAWN]   = offsetof(max_plist_t, pawn),
        [MAX_PIECECODE_KNIGHT] = offsetof(max_plist_t, knight),
        [MAX_PIECECODE_BISHOP] = offsetof(max_plist_t, bishop),
        [MAX_PIECECODE_ROOK]   = offsetof(max_plist_t, rook),
        [MAX_PIECECODE_QUEEN]  = offsetof(max_plist_t, queen),
        [MAX_PIECECODE_KING]   = offsetof(max_plist_t, king)
    };

    return (max_loclist_t*)(((void*)pieces) + OFFSETS[piece.v * MAX_PIECECODE_TYPE_MASK]);
}

/// Add a board location to the given locations list.
/// No capacity checks are performed here, even with debug assertions enabled
/// (the capacity is not known alone, we must also know the piece type first)
MAX_INLINE_ALWAYS void max_loclist_add(max_loclist_t *pieces, max_0x88_t pos) {
    pieces->loc[pieces->len] = pos;
    pieces->len += 1;
}

/// @}

/// @}
