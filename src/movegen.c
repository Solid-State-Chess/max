#include "max.h"
#include "max/board.h"
#include "max/def.h"
#include "max/move.h"
#include "max/piece.h"
#include "max/square.h"
#include "private.h"
#include <stdbool.h>

/// Generate a sliding attack in the given direction
static MAX_INLINE_ALWAYS void max_slidegen(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bidx_t start,
    max_increment_t dir
) {
    max_bidx_t pos = start;
    for(;;) {
        pos = max_bidx_inc(pos, dir);
        if(!max_bidx_valid(pos)) { break; }
        max_piececode_t square = board->pieces[pos];
        if(square == MAX_PIECECODE_EMPTY) {
            max_movelist_add(moves, max_move_normal(start, pos));
        } else {
            if(square & enemy) {
                max_movelist_add(moves, max_move_capture(start, pos));
            }
            break;
        }
    }
}

static MAX_INLINE_ALWAYS void max_bishopgen(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bidx_t start
) {
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_UP + MAX_INCREMENT_RIGHT);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_DOWN + MAX_INCREMENT_RIGHT);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_UP + MAX_INCREMENT_LEFT);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_DOWN + MAX_INCREMENT_LEFT);
}

static MAX_INLINE_ALWAYS void max_rookgen(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bidx_t start
) {
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_UP);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_RIGHT);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_DOWN);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_LEFT);
}

void max_movegen(max_movelist_t *const moves, max_board_t *const board) {
    //Homerow lookup table, indexed by the current side to move
    static max_bidx_t PAWN_HOMEROW[2]  = {MAX_RANK_2, MAX_RANK_7};
    //Rank that an enemy pawn would be en passanted at
    static max_bidx_t PAWN_EPRANK[2]   = {MAX_RANK_5, MAX_RANK_4};
    //Pawn travel direction indexed by the side to move
    
    uint8_t side = board->ply & 1;
    max_sidestate_t *state = &board->sides[side];
    max_sidestate_t *enemy = &board->sides[side ^ 1];

    max_piececode_t enemy_color = MAX_PIECECODE_BLACK >> side;


    max_bidx_t pawn_homerow = PAWN_HOMEROW[side];
    //If the EP file is invalid on the stack, then this will be an invalid index
    max_bidx_t epsquare = (board->stack[board->ply] & MAX_PLYPLATE_EP_MASK) | PAWN_EPRANK[side];
    for(max_lidx_t i = 0; i < state->piecelist.pawns.len; ++i) {
        max_bidx_t pos = state->piecelist.pawns.pos[i];
        max_bidx_t up = max_bidx_inc(pos, PAWN_INC[side]);

        if(max_bidx_valid(up)) {
            if(board->pieces[up] == MAX_PIECECODE_EMPTY) {
                max_movelist_add(moves, max_move_normal(pos, up));
                max_bidx_t up2 = max_bidx_inc(up, PAWN_INC[side]);
                if((pos & MAX_RANK_MASK) == PAWN_HOMEROW[side] && board->pieces[up2] == MAX_PIECECODE_EMPTY) {
                    max_movelist_add(moves, max_move_new(pos, up2, MAX_MOVE_DOUBLE));
                }
            }

            max_bidx_t right = max_bidx_inc(up, MAX_INCREMENT_RIGHT);
            if(max_bidx_valid(right) && (board->pieces[right] & enemy_color)) {
                max_movelist_add(moves, max_move_capture(pos, right));
            }

            max_bidx_t left = max_bidx_inc(up, MAX_INCREMENT_LEFT);
            if(max_bidx_valid(left) && (board->pieces[left] & enemy_color)) {
                max_movelist_add(moves, max_move_capture(pos, left));
            }

            if(max_bidx_valid(epsquare)) {
                if(max_bidx_inc(pos, MAX_INCREMENT_RIGHT) == epsquare || max_bidx_inc(pos, MAX_INCREMENT_LEFT) == epsquare) {
                    max_bidx_t moveto = max_bidx_inc(epsquare, PAWN_INC[side]);
                    max_movelist_add(moves, max_move_new(pos, moveto, MAX_MOVE_EN_PASSANT));
                }
            }
        }

    }

    #define NORMALMOVE(to) do {                                       \
        max_bidx_t dest = (to);                                       \
        if(max_bidx_valid(dest)) {                                    \
            max_piececode_t piece = board->pieces[dest];         \
            if(piece == MAX_PIECECODE_EMPTY) {                        \
                max_movelist_add(moves, max_move_normal(pos, dest));  \
            } else if(piece & enemy_color) {                          \
                max_movelist_add(moves, max_move_capture(pos, dest)); \
            }                                                         \
        }                                                             \
    } while(0)

    for(max_lidx_t i = 0; i < state->piecelist.knights.len; ++i) {
        max_bidx_t pos = state->piecelist.knights.pos[i];
        NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UP + MAX_INCREMENT_RIGHT));
        NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UP + MAX_INCREMENT_LEFT));

        NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DOWN + MAX_INCREMENT_RIGHT));
        NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DOWN + MAX_INCREMENT_LEFT));

        NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_RIGHT + MAX_INCREMENT_UP));
        NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_RIGHT + MAX_INCREMENT_DOWN));


        NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_LEFT + MAX_INCREMENT_UP));
        NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_LEFT + MAX_INCREMENT_DOWN));
    }

    for(max_lidx_t i = 0; i < state->piecelist.bishops.len; ++i) {
        max_bishopgen(moves, board, enemy_color, state->piecelist.bishops.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->piecelist.rooks.len; ++i) {
        max_rookgen(moves, board, enemy_color, state->piecelist.rooks.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->piecelist.queens.len; ++i) {
        max_rookgen(moves, board, enemy_color, state->piecelist.queens.pos[i]);
        max_bishopgen(moves, board, enemy_color, state->piecelist.queens.pos[i]);
    }
   
    max_bidx_t pos = state->piecelist.king.pos[0];
    NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_UP));
    NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_DOWN));
    NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_LEFT));
    NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_RIGHT));
    NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_LEFT));
    NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_RIGHT));
    NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_LEFT));
    NORMALMOVE(max_bidx_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_RIGHT));
}
