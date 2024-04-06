#pragma once

#include "max/board.h"
#include "max/def.h"
#include "max/square.h"

/// Increment for a pawn indexed by side bit
extern const max_increment_t PAWN_INC[2];

extern const max_increment_t KNIGHT_MOVES[8];

void max_board_init_statics(max_board_t *const board);

/// Get the queenside castle right flag bits for the side to move on the given ply
MAX_INLINE_ALWAYS max_plyplate_t max_qcastle_flag(uint16_t ply) {
    return (MAX_PLYPLATE_KCASTLE << MAX_PLYPLATE_CASTLE_OFFSET) << ((ply & 1) << 1);
}

/// Get the kingside castle flag bits for the side to move on the given ply
MAX_INLINE_ALWAYS max_plyplate_t max_kcastle_flag(uint16_t ply) {
    return (MAX_PLYPLATE_QCASTLE << MAX_PLYPLATE_CASTLE_OFFSET) << ((ply & 1) << 1);
}
