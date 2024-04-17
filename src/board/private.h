#pragma once

#include "max/board/board.h"
#include "max/board/zobrist.h"
#include "max/def.h"
#include "max/board/square.h"

/// Update check data for the current side to play
void max_board_update_check(max_board_t *const board, max_move_t move, max_check_t *check);

/// Generate a random number according to the random seed in the given board
max_zobrist_t max_zobrist_rand(max_board_t *const board);

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

    board->zobrist.hash ^= max_zobrist_piecepos(piece, pos);
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

    board->zobrist.hash ^= max_zobrist_piecepos(piece, pos);
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
    board->zobrist.hash ^= max_zobrist_piecepos(piece, from);
    board->zobrist.hash ^= max_zobrist_piecepos(piece, to);
}
