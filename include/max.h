#pragma once
#include "max/board.h"
#include "max/move.h"

struct engine {

};

/// Generate all valid moves for the current side to move on the given board
void max_movegen(max_movelist_t *const moves, max_board_t *const board);
