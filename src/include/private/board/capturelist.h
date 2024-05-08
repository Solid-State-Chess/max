#pragma once
#include "max/board/capturelist.h"


/// \ingroup state
/// @{

/// \name Private Functions

/// Initialize the given capture list's length to 0 without modifying the contents.
MAX_INLINE_ALWAYS void max_captures_new(max_captures_t *caps) { caps->len = 0; }

/// Add a piece to the top of the given capture state.
/// With debug assertions enabled, this function will be bounds checked to ensure we don't
/// overrun the end of the buffer.
MAX_INLINE_ALWAYS void max_captures_add(max_captures_t *caps, max_piececode_t piece) {
    MAX_ASSERT(caps->len < MAX_BOARD_PIECES_COUNT);
    caps->pieces[caps->len] = piece;
    caps->len += 1;
}

/// Pop the top element from the capture stack and return it.
/// With debug assertions enabled, this will bounds check the capture stack to ensure we return a valid piece.
MAX_INLINE_ALWAYS max_piececode_t max_captures_pop(max_captures_t *caps) {
    MAX_ASSERT(caps->len > 0);
    caps->len -= 1;
    return caps->pieces[caps->len];
}

/// @}

/// @}
