#pragma once

#include "max/board.h"
#include "max/def.h"
#include "max/square.h"

/// Update check data for the current side to play
void max_board_update_check(max_board_t *const board, max_move_t move);

void max_board_unupdate_check(max_board_t *const board, max_move_t move);

/// Get the queenside castle right flag bits for the side to move on the given ply
MAX_INLINE_ALWAYS max_plyplate_t max_qcastle_flag(uint16_t ply) {
    return (MAX_PLYPLATE_KCASTLE << MAX_PLYPLATE_CASTLE_OFFSET) << ((ply & 1) << 1);
}

/// Get the kingside castle flag bits for the side to move on the given ply
MAX_INLINE_ALWAYS max_plyplate_t max_kcastle_flag(uint16_t ply) {
    return (MAX_PLYPLATE_QCASTLE << MAX_PLYPLATE_CASTLE_OFFSET) << ((ply & 1) << 1);
}

/// Remove a piece by position from the given side
MAX_HOT
MAX_INLINE_ALWAYS void max_board_remove_piece(max_board_t *board, max_pieces_t *side, max_bpos_t pos) {
    max_piececode_t piece = board->pieces[pos];
    board->pieces[pos] = MAX_PIECECODE_EMPTY;
    max_piecelist_t *list = max_pieces_get_list(side, piece);
    max_lidx_t index = side->index[pos];
    
    //Replace the removed element with the last element of the list
    list->len -= 1;
    max_bpos_t shuffled_pos = list->pos[list->len];
    list->pos[index] = shuffled_pos;
    side->index[shuffled_pos] = index;
}

/// Add a piece with the given code to the chessboard
MAX_HOT
MAX_INLINE_ALWAYS void max_board_add_piece(max_board_t *board, max_pieces_t *side, max_bpos_t pos, max_piececode_t piece) {
    board->pieces[pos] = piece;
    max_piecelist_t *list = max_pieces_get_list(side, piece);
    max_lidx_t index = list->len;
    list->pos[list->len] = pos;
    list->len += 1;

    side->index[pos] = index;
}

/// Shift the piece from the given location to the given location, replacing its old position with an empty square
MAX_HOT
MAX_INLINE_ALWAYS void max_board_shift_piece(max_board_t *board, max_pieces_t *side, max_bpos_t from, max_bpos_t to) {
    max_piececode_t piece = board->pieces[from];
    max_lidx_t index = side->index[from];
    max_piecelist_t *list = max_pieces_get_list(side, piece);
    list->pos[index] = to;
    side->index[to] = index;
    board->pieces[to] = piece;
    
    board->pieces[from] = MAX_PIECECODE_EMPTY;
}
