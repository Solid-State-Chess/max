#pragma once
#include "max/board/piece.h"
#include "max/board/board.h"

void max_board_pawnmovegen_quiet(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_piececode_t enemy,
    uint8_t side,
    max_bpos_t pos
);

bool max_board_pawn_promotegen(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_bpos_t pos,
    max_bpos_t square,
    uint8_t side,
    max_move_attr_t flags
);

void max_board_pawnmovegen_loud(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_piececode_t enemy_color,
    uint8_t side,
    max_bpos_t pos
);
