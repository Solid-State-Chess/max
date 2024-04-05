#include "max/board.h"
#include "max/def.h"
#include "max/move.h"
#include "max/piece.h"
#include "max/square.h"
#include "private.h"
#include <string.h>

const max_increment_t PAWN_INC[2] = {MAX_INCREMENT_UP, MAX_INCREMENT_DOWN};

static void max_board_postinit(max_board_t *const board);

void max_board_new(max_board_t *const board) {
    board->ply = 0;
    memset(&board->pieces, MAX_PIECECODE_INVAL, sizeof(board->pieces));
    memset(board->sides, 0, sizeof(board->sides));
    memset(&board->captures, 0, sizeof(board->captures));
    board->stack[0] = MAX_PLYPLATE_WCASTLEMASK | MAX_PLYPLATE_BCASTLEMASK | MAX_PLYPLATE_EP_INVALID;
    
    for(unsigned i = MAX_A2; i <= MAX_H2; i += 1) {
        board->pieces[i] = MAX_PIECECODE_PAWN | MAX_PIECECODE_WHITE;
    }

    for(unsigned i = MAX_A7; i <= MAX_H7; i += 1) {
        board->pieces[i] = MAX_PIECECODE_PAWN | MAX_PIECECODE_BLACK;
    }

    #define SYMMETRY(r1, r2, kind) do {                                               \
        board->pieces[MAX_##r1##1] = board->pieces[MAX_##r2##1] = kind | MAX_PIECECODE_WHITE; \
        board->pieces[MAX_##r1##8] = board->pieces[MAX_##r2##8] = kind | MAX_PIECECODE_BLACK; \
    } while(0)

    SYMMETRY(A, H, MAX_PIECECODE_ROOK); 
    SYMMETRY(B, G, MAX_PIECECODE_KNIGHT);
    SYMMETRY(C, F, MAX_PIECECODE_BISHOP);

    #undef SYMMETRY

    board->pieces[MAX_D1] = MAX_PIECECODE_QUEEN | MAX_PIECECODE_WHITE;
    board->pieces[MAX_D8] = MAX_PIECECODE_QUEEN | MAX_PIECECODE_BLACK;

    board->pieces[MAX_E1] = MAX_PIECECODE_KING | MAX_PIECECODE_WHITE;
    board->pieces[MAX_E8] = MAX_PIECECODE_KING | MAX_PIECECODE_BLACK;

    max_board_postinit(board);
}

/// Initialize piece lists for each side by iterating through each square and set all valid empty squares to the proper piece code
static void max_board_postinit(max_board_t *const board) {
    for(uint8_t x = 0; x < 8; ++x) {
        for(uint8_t y = 0; y < 8; ++y) {
            max_bpos_t pos = max_bpos_new(x, y);
            max_piececode_t piece = board->pieces[pos];
            
            if(piece == MAX_PIECECODE_INVAL) {
                board->pieces[pos] = MAX_PIECECODE_EMPTY;
                continue;
            }
            
            max_pieces_t *side = &board->sides[(piece & MAX_PIECECODE_BLACK) >> 6];
            max_board_add_piece(board, side, pos, piece);
        }
    }
}

/// Perform the given pawn promotion move
MAX_HOT
MAX_INLINE_ALWAYS
void max_board_promote(max_board_t *const board, max_pieces_t *side, max_move_t move, max_piececode_t piece, max_piececode_t promoted) {
    max_board_remove_piece(board, side, move.from);
    max_board_add_piece(board, side, promoted, move.to);

}

/// Perform the given pawn promotion move
MAX_HOT
MAX_INLINE_ALWAYS
void max_board_unpromote(max_board_t *const board, max_pieces_t *side, max_move_t move, max_piececode_t color) {
    max_board_remove_piece(board, side, move.to);
    max_board_add_piece(board, side, MAX_PIECECODE_PAWN | color, move.from);
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



    switch(move.attr) {
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
        case MAX_MOVE_CAPTURE: {
            max_piececode_t captured = board->pieces[move.to];
            max_capturestack_push(&board->captures, captured);
            max_board_remove_piece(board, &board->sides[side_to_move ^ 1], move.to);
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

    switch(move.attr) {
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
            max_board_add_piece(
                board,
                &board->sides[side_to_move ^ 1],
                move.to,
                max_capturestack_pop(&board->captures)
            );
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

MAX_HOT
MAX_INLINE_ALWAYS
bool max_board_valid_line(max_board_t *const board, max_piececode_t piece, max_bpos_t attacked, max_bpos_t to) {
    static const max_increment_t QUEENLINES[240] = {0};

    max_bpos_t diff = 0x77 + to - attacked;
    max_increment_t line = QUEENLINES[diff];
    if(line != 0) {
        max_bpos_t pos = max_bpos_inc(attacked, line);
        while(max_bpos_valid(pos) && (board->pieces[pos] == MAX_PIECECODE_EMPTY)) {
            pos = max_bpos_inc(pos, line);
        }

        if((board->pieces[pos] & (piece & MAX_PIECECODE_COLOR_MASK)) == 0) {
            return false;
        }
    }
    
    return true;
}

MAX_HOT
bool max_board_move_is_valid(max_board_t *const board, max_move_t move) {
    static max_move_t BUFFER[256] = {0};
    max_movelist_t moves = max_movelist_new(BUFFER);
    
    max_board_make_move(board, move);
    
    
    max_bpos_t kpos = board->sides[(board->ply & 1) ^ 1].king.pos[0];
    max_board_movegen_pseudo(board, &moves);

    for(unsigned i = 0; i < moves.len; ++i) {
        if(moves.moves[i].to == kpos) {
            max_board_unmake_move(board, move);
            return false;
        }        
    }
    
    max_board_unmake_move(board, move);
    return true;

    /*max_piececode_t piece = board->pieces[move.from];
    if((piece & MAX_PIECECODE_KING)) {
        if(max_board_square_is_attacked(board, move.to, (MAX_PIECECODE_WHITE << board->ply) & 1)) {
            return false;
        }
    } else if(move.attr == MAX_MOVE_EN_PASSANT) {
    } else {
        return max_board_valid_line(board, piece, move.from, kpos);
    }*/
}


/// Check if the given piece is attacked by the given side
bool max_board_square_is_attacked(max_board_t *const board, max_bpos_t square, max_piececode_t color_mask) {
    
}
