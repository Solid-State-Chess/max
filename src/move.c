#include "max/move.h"
#include "max/board.h"
#include "max/def.h"
#include "private.h"

/// Perform the given pawn promotion move
MAX_HOT
MAX_INLINE_ALWAYS
void max_board_promote(max_board_t *const board, max_pieces_t *side, max_move_t move, max_piececode_t piece, max_piececode_t promoted) {
    if(move.attr & MAX_MOVE_CAPTURE) {
        max_capturestack_push(&board->captures, board->pieces[move.to]);
        max_board_remove_piece(board, max_board_get_enemy(board), move.to);
    }
    max_board_remove_piece(board, side, move.from);
    max_board_add_piece(board, side, move.to, promoted);

}

/// Perform the given pawn promotion move
MAX_HOT
MAX_INLINE_ALWAYS
void max_board_unpromote(max_board_t *const board, max_pieces_t *side, max_move_t move, max_piececode_t color) {
    max_board_remove_piece(board, side, move.to);
    max_board_add_piece(board, side, move.from, MAX_PIECECODE_PAWN | color);
    if(move.attr & MAX_MOVE_CAPTURE) {
        max_board_add_piece(board, max_board_get_enemy(board), move.to, max_capturestack_pop(&board->captures));
    }
}

MAX_HOT
void max_board_make_move(max_board_t *const board, max_move_t move) {
    static uint8_t KCASTLE_ROOK_SQUARE[2] = {MAX_H1, MAX_H8};
    static uint8_t QCASTLE_ROOK_SQUARE[2] = {MAX_A1, MAX_A8};

    max_piececode_t piece = board->pieces[move.from];
    //0 if white, 1 if black
    uint8_t side_to_move = board->ply & 1;
    max_pieces_t *side = &board->sides[side_to_move];

    max_plyplate_t state_plate = board->stack[board->ply] | MAX_PLYPLATE_EP_INVALID;
    if(piece & MAX_PIECECODE_KING) {
        state_plate &= ~(MAX_PLYPLATE_WCASTLEMASK << (side_to_move << 1));
    } else {
        if(piece & MAX_PIECECODE_ROOK) {
            if(move.from == KCASTLE_ROOK_SQUARE[side_to_move]) {
                state_plate &= ~max_kcastle_flag(board->ply);
            } else if(move.from == QCASTLE_ROOK_SQUARE[side_to_move]) {
                state_plate &= ~max_qcastle_flag(board->ply);
            }
        }
    }
    
    if(move.attr == MAX_MOVE_CAPTURE) {
        max_piececode_t captured = board->pieces[move.to];
        max_capturestack_push(&board->captures, captured);
        max_board_remove_piece(board, &board->sides[side_to_move ^ 1], move.to);
    }


    switch(move.attr & ~MAX_MOVE_CAPTURE) {
        case MAX_MOVE_NORMAL: break;
        case MAX_MOVE_DOUBLE: {
            state_plate &= ~(MAX_PLYPLATE_EP_INVALID);
            //Set the en-passant file
            state_plate |= move.from & 7;
        } break;

        case MAX_MOVE_PROMOTE_KNIGHT: max_board_promote(board, side, move, piece, MAX_PIECECODE_KNIGHT | (piece & MAX_PIECECODE_COLOR_MASK)); goto end; break;
        case MAX_MOVE_PROMOTE_BISHOP: max_board_promote(board, side, move, piece, MAX_PIECECODE_BISHOP | (piece & MAX_PIECECODE_COLOR_MASK)); goto end; break;
        case MAX_MOVE_PROMOTE_ROOK:   max_board_promote(board, side, move, piece, MAX_PIECECODE_ROOK   | (piece & MAX_PIECECODE_COLOR_MASK)); goto end; break;
        case MAX_MOVE_PROMOTE_QUEEN:  max_board_promote(board, side, move, piece, MAX_PIECECODE_QUEEN  | (piece & MAX_PIECECODE_COLOR_MASK)); goto end; break;

        case MAX_MOVE_EN_PASSANT: {
            max_bpos_t takeat = max_bpos_inc(move.to, PAWN_INC[side_to_move ^ 1]);
            max_piececode_t captured = board->pieces[takeat];
            max_capturestack_push(&board->captures, captured);
            max_board_remove_piece(board, &board->sides[side_to_move ^ 1], takeat);
        } break;

        case MAX_MOVE_KCASTLE: {
            max_bpos_t old_rook_pos = max_bpos_inc(move.to, MAX_INCREMENT_RIGHT);
            max_bpos_t rook_pos = max_bpos_inc(move.to, MAX_INCREMENT_LEFT);
            max_board_shift_piece(board, side, old_rook_pos, rook_pos);
        } break;

        case MAX_MOVE_QCASTLE: {
            max_bpos_t old_rook_pos = max_bpos_inc(move.to, MAX_INCREMENT_LEFT + MAX_INCREMENT_LEFT);
            max_bpos_t rook_pos     = max_bpos_inc(move.to, MAX_INCREMENT_RIGHT);
            max_board_shift_piece(board, side, old_rook_pos, rook_pos);
        } break;
    }

    max_board_shift_piece(board, side, move.from, move.to);

end:
    board->ply += 1;
    board->stack[board->ply] = state_plate;

    #undef SWAPROOK
    #undef CAPTUREPIECE
}

MAX_HOT
void max_board_unmake_move(max_board_t *const board, max_move_t move) {
    board->ply -= 1;
    uint8_t side_to_move = board->ply & 1;
    max_piececode_t piece = board->pieces[move.to];
    max_pieces_t *side = &board->sides[side_to_move];
    max_board_shift_piece(board, side, move.to, move.from);

    if(move.attr == MAX_MOVE_CAPTURE) {
        max_board_add_piece(
            board,
            &board->sides[side_to_move ^ 1],
            move.to,
            max_capturestack_pop(&board->captures)
        );
    }

    switch(move.attr & ~MAX_MOVE_CAPTURE) {
        case MAX_MOVE_NORMAL:
        case MAX_MOVE_DOUBLE: {
        } break;
        
        case MAX_MOVE_PROMOTE_KNIGHT:
        case MAX_MOVE_PROMOTE_BISHOP:
        case MAX_MOVE_PROMOTE_ROOK:
        case MAX_MOVE_PROMOTE_QUEEN: {
            max_board_unpromote(board, side, move, piece & MAX_PIECECODE_COLOR_MASK);
            return;
        }


        case MAX_MOVE_EN_PASSANT: {
            max_bpos_t takeat = max_bpos_inc(move.to, PAWN_INC[side_to_move ^ 1]);
            max_board_add_piece(
                board,
                &board->sides[side_to_move ^ 1],
                takeat,
                max_capturestack_pop(&board->captures)
            );
        } break;
        case MAX_MOVE_CAPTURE: {
            /*max_board_add_piece(
                board,
                &board->sides[side_to_move ^ 1],
                move.to,
                max_capturestack_pop(&board->captures)
            );*/
        } break;

        case MAX_MOVE_KCASTLE: {
            max_bpos_t old_rook_pos = max_bpos_inc(move.to, MAX_INCREMENT_RIGHT);
            max_bpos_t rook_pos = max_bpos_inc(move.to, MAX_INCREMENT_LEFT);
            max_board_shift_piece(board, side, rook_pos, old_rook_pos);
        } break;

        case MAX_MOVE_QCASTLE: {
            max_bpos_t old_rook_pos = max_bpos_inc(move.to, MAX_INCREMENT_LEFT + MAX_INCREMENT_LEFT);
            max_bpos_t rook_pos     = max_bpos_inc(move.to, MAX_INCREMENT_RIGHT);
            max_board_shift_piece(board, side, rook_pos, old_rook_pos);
        } break;
    }
}
