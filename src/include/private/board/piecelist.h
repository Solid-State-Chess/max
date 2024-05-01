#pragma once

#include "max/board/piecelist.h"

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

/// @}

/// @}
