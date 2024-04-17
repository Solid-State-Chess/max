#pragma once

#include "max/board/movegen.h"
#include "max/board/board.h"
#include "max/board/move.h"

/// Internal function to check a move for validity and add it to a moves list
MAX_INLINE_ALWAYS void max_board_addmove(max_board_t *board, max_movelist_t *moves, max_move_t move) {
    if(max_board_move_is_valid(board, move)) {
        max_movelist_add(moves, move);
    }
}
