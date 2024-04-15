#include "max/board/board.h"
#include "max/def.h"
#include "max/board/move.h"
#include "max/board/piece.h"
#include "max/board/square.h"
#include <stdbool.h>
#include "pawn.h"
#include "slider.h"


MAX_HOT
void max_board_movegen_pseudo(max_board_t *const board, max_movelist_t *const moves) {
    uint8_t side = board->ply & 1;
    max_pieces_t *state = &board->sides[side];
    max_pieces_t *enemy = &board->sides[side ^ 1];

    max_piececode_t enemy_color = MAX_PIECECODE_BLACK >> side;

    for(max_lidx_t i = 0; i < state->pawns.len; ++i) {
        max_bpos_t pos = state->pawns.pos[i];
        max_board_pawnmovegen_quiet(board, moves, enemy_color, side, pos);
        max_board_pawnmovegen_loud(board, moves, enemy_color, side, pos); 
    }

    #define NORMALMOVE(from, to) do {                                       \
        max_bpos_t dest = (to);                                             \
        if(max_bpos_valid(dest)) {                                          \
            max_piececode_t piece = board->pieces[dest];                    \
            if(piece == MAX_PIECECODE_EMPTY) {                              \
                max_movelist_add(moves, max_move_normal((from), dest));     \
            } else if(piece & enemy_color) {                                \
                max_movelist_add(moves, max_move_capture((from), dest));    \
            }                                                               \
        }                                                                   \
    } while(0)

    for(max_lidx_t i = 0; i < state->knights.len; ++i) {
        max_bpos_t pos = state->knights.pos[i];
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UL));

        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DL));

        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_DR));


        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_UL));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_DL));
    }

    for(max_lidx_t i = 0; i < state->bishops.len; ++i) {
        max_bishopgen_all(moves, board, enemy_color, state->bishops.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->rooks.len; ++i) {
        max_rookgen_all(moves, board, enemy_color, state->rooks.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->queens.len; ++i) {
        max_rookgen_all(moves, board, enemy_color, state->queens.pos[i]);
        max_bishopgen_all(moves, board, enemy_color, state->queens.pos[i]);
    }
    
    if(state->king.len != 0) {
        max_bpos_t kingpos = state->king.pos[0];
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UP));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DOWN));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_LEFT));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UR));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UL));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DR));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DL));
        
        max_plyplate_t plate = max_board_state(board)->packed_state;
        if(plate & max_kcastle_flag(board->ply)) {
            max_piececode_t color = MAX_PIECECODE_WHITE << side;

            max_bpos_t between[3] = {
                max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT),
                max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT * 2),
                max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT * 3),
            };
                    if(board->pieces[between[0]] == MAX_PIECECODE_EMPTY && board->pieces[between[1]] == MAX_PIECECODE_EMPTY && board->pieces[between[2]] == (MAX_PIECECODE_ROOK | color)) {
                max_movelist_add(moves, max_move_new(kingpos, between[1], MAX_MOVE_KCASTLE));
            }
        }
 
        if(plate & max_qcastle_flag(board->ply)) {
            max_piececode_t color = MAX_PIECECODE_WHITE << side;
            max_bpos_t between[4] = {
                max_bpos_inc(kingpos, MAX_INCREMENT_LEFT),
                max_bpos_inc(kingpos, MAX_INCREMENT_LEFT * 2),
                max_bpos_inc(kingpos, MAX_INCREMENT_LEFT * 3),
                max_bpos_inc(kingpos, MAX_INCREMENT_LEFT * 4)
            };

            if(
                board->pieces[between[0]] == MAX_PIECECODE_EMPTY &&
                board->pieces[between[1]] == MAX_PIECECODE_EMPTY &&
                board->pieces[between[2]] == MAX_PIECECODE_EMPTY &&
                board->pieces[between[3]] == (MAX_PIECECODE_ROOK | color)
            ) {
                max_movelist_add(moves, max_move_new(kingpos, between[1], MAX_MOVE_QCASTLE));
            }
        }
    }

    #undef NORMALMOVE
}


/// Generate all pseudo-legal capture moves for the side to play
void max_board_capturegen_pseudo(max_board_t *const board, max_movelist_t *const moves) {
    uint8_t side = board->ply & 1;
    max_pieces_t *state = &board->sides[side];

    max_piececode_t enemy_color = MAX_PIECECODE_BLACK >> side;

    for(max_lidx_t i = 0; i < state->pawns.len; ++i) {
        max_bpos_t pos = state->pawns.pos[i];
        max_board_pawnmovegen_loud(board, moves, enemy_color, side, pos); 
    }

    #define NORMALMOVE(from, to) do {                                       \
        max_bpos_t dest = (to);                                             \
        if(max_bpos_valid(dest)) {                                          \
            max_piececode_t piece = board->pieces[dest];                    \
            if(piece & enemy_color) {                                       \
                max_movelist_add(moves, max_move_capture((from), dest));    \
            }                                                               \
        }                                                                   \
    } while(0)

    for(max_lidx_t i = 0; i < state->knights.len; ++i) {
        max_bpos_t pos = state->knights.pos[i];
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UL));

        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DL));

        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_DR));


        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_UL));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_DL));
    }

    for(max_lidx_t i = 0; i < state->bishops.len; ++i) {
        max_bishopgen_loud(moves, board, enemy_color, state->bishops.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->rooks.len; ++i) {
        max_rookgen_loud(moves, board, enemy_color, state->rooks.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->queens.len; ++i) {
        max_rookgen_loud(moves, board, enemy_color, state->queens.pos[i]);
        max_bishopgen_loud(moves, board, enemy_color, state->queens.pos[i]);
    }
    
    if(state->king.len != 0) {
        max_bpos_t kingpos = state->king.pos[0];
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UP));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DOWN));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_LEFT));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UR));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UL));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DR));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DL));
    }
}
