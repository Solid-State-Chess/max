#include "max/board/piece.h"
#include "max/board/board.h"


MAX_HOT void max_board_pawnmovegen_quiet(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_piececode_t enemy,
    uint8_t side,
    max_bpos_t pos
) {
    max_bpos_t up = max_bpos_inc(pos, MAX_PAWN_DIR[side]);

    if(max_bpos_valid(up)) {
        if((up & MAX_RANK_MASK) != PAWN_PROMOTERANK[side] && board->pieces[up] == MAX_PIECECODE_EMPTY) {
            max_movelist_add(moves, max_move_normal(pos, up));
            max_bpos_t up2 = max_bpos_inc(up, MAX_PAWN_DIR[side]);
            if((pos & MAX_RANK_MASK) == PAWN_HOMEROW[side] && board->pieces[up2] == MAX_PIECECODE_EMPTY) {
                max_movelist_add(moves, max_move_new(pos, up2, MAX_MOVE_DOUBLE));
            }
        }
    }
}

bool max_board_pawn_promotegen(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_bpos_t pos,
    max_bpos_t square,
    uint8_t side,
    max_move_attr_t flags
) {
    if(max_bpos_valid(square) && (square & MAX_RANK_MASK) == PAWN_PROMOTERANK[side]) {
        max_movelist_add(moves, max_move_new(pos, square, flags | MAX_MOVE_PROMOTE_KNIGHT));
        max_movelist_add(moves, max_move_new(pos, square, flags | MAX_MOVE_PROMOTE_BISHOP));
        max_movelist_add(moves, max_move_new(pos, square, flags | MAX_MOVE_PROMOTE_ROOK));
        max_movelist_add(moves, max_move_new(pos, square, flags | MAX_MOVE_PROMOTE_QUEEN));
        return true;
    }

    return false;
}

MAX_HOT void max_board_pawnmovegen_loud(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_piececode_t enemy_color,
    uint8_t side,
    max_bpos_t pos
) {
    max_bpos_t up = max_bpos_inc(pos, MAX_PAWN_DIR[side]);
    if(max_bpos_valid(up) && board->pieces[up] == MAX_PIECECODE_EMPTY) {
        max_board_pawn_promotegen(board, moves, pos, up, side, 0);
    }
    
    max_bpos_t right = max_bpos_inc(up, MAX_INCREMENT_RIGHT);
    if(max_bpos_valid(right) && (board->pieces[right] & enemy_color)) {
        if(!max_board_pawn_promotegen(board, moves, pos, right, side, MAX_MOVE_CAPTURE)) {
            max_movelist_add(moves, max_move_capture(pos, right));
        }
    }

    max_bpos_t left = max_bpos_inc(up, MAX_INCREMENT_LEFT);
    if(max_bpos_valid(left) && (board->pieces[left] & enemy_color)) {
        if(!max_board_pawn_promotegen(board, moves, pos, left, side, MAX_MOVE_CAPTURE)) {
            max_movelist_add(moves, max_move_capture(pos, left));
        }
    }
    

    //If the EP file is invalid on the stack, then this will be an invalid index
    max_bpos_t epsquare = (max_board_state(board)->packed_state & MAX_PLYPLATE_EP_MASK) | PAWN_EPRANK[side];

    if(max_bpos_valid(epsquare)) {
        if(max_bpos_inc(pos, MAX_INCREMENT_RIGHT) == epsquare || max_bpos_inc(pos, MAX_INCREMENT_LEFT) == epsquare) {
            max_bpos_t moveto = max_bpos_inc(epsquare, MAX_PAWN_DIR[side]);
            max_movelist_add(moves, max_move_new(pos, moveto, MAX_MOVE_EN_PASSANT));
        }
    }
}
