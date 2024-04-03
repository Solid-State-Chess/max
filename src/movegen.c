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
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_UR);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_DR);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_UL);
    max_slidegen(moves, board, enemy, start, MAX_INCREMENT_DL);
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

MAX_HOT
void max_board_movegen_pseudo(max_board_t *const board, max_movelist_t *const moves) {
    //Homerow lookup table, indexed by the current side to move
    static max_bidx_t PAWN_HOMEROW[2]  = {MAX_RANK_2, MAX_RANK_7};
    //Rank that an enemy pawn would be en passanted at
    static max_bidx_t PAWN_EPRANK[2]   = {MAX_RANK_5, MAX_RANK_4};
    
    uint8_t side = board->ply & 1;
    max_pieces_t *state = &board->sides[side];
    max_pieces_t *enemy = &board->sides[side ^ 1];

    max_piececode_t enemy_color = MAX_PIECECODE_BLACK >> side;
    max_bidx_t pawn_homerow = PAWN_HOMEROW[side];

    //If the EP file is invalid on the stack, then this will be an invalid index
    max_bidx_t epsquare = (board->stack[board->ply] & MAX_PLYPLATE_EP_MASK) | PAWN_EPRANK[side];

    for(max_lidx_t i = 0; i < state->pawns.len; ++i) {
        max_bidx_t pos = state->pawns.pos[i];
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

    #define NORMALMOVE(from, to) do {                                       \
        max_bidx_t dest = (to);                                       \
        if(max_bidx_valid(dest)) {                                    \
            max_piececode_t piece = board->pieces[dest];              \
            if(piece == MAX_PIECECODE_EMPTY) {                        \
                max_movelist_add(moves, max_move_normal((from), dest));  \
            } else if(piece & enemy_color) {                          \
                max_movelist_add(moves, max_move_capture((from), dest)); \
            }                                                         \
        }                                                             \
    } while(0)

    for(max_lidx_t i = 0; i < state->knights.len; ++i) {
        max_bidx_t pos = state->knights.pos[i];
        NORMALMOVE(pos, max_bidx_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bidx_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UL));

        NORMALMOVE(pos, max_bidx_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DR));
        NORMALMOVE(pos, max_bidx_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DL));

        NORMALMOVE(pos, max_bidx_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bidx_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_DR));


        NORMALMOVE(pos, max_bidx_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_UL));
        NORMALMOVE(pos, max_bidx_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_DL));
    }

    for(max_lidx_t i = 0; i < state->bishops.len; ++i) {
        max_bishopgen(moves, board, enemy_color, state->bishops.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->rooks.len; ++i) {
        max_rookgen(moves, board, enemy_color, state->rooks.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->queens.len; ++i) {
        max_rookgen(moves, board, enemy_color, state->queens.pos[i]);
        max_bishopgen(moves, board, enemy_color, state->queens.pos[i]);
    }
   
    max_bidx_t kingpos = state->king.pos[0];
    NORMALMOVE(kingpos, max_bidx_inc(kingpos, MAX_INCREMENT_UP));
    NORMALMOVE(kingpos, max_bidx_inc(kingpos, MAX_INCREMENT_DOWN));
    NORMALMOVE(kingpos, max_bidx_inc(kingpos, MAX_INCREMENT_LEFT));
    NORMALMOVE(kingpos, max_bidx_inc(kingpos, MAX_INCREMENT_RIGHT));
    NORMALMOVE(kingpos, max_bidx_inc(kingpos, MAX_INCREMENT_UR));
    NORMALMOVE(kingpos, max_bidx_inc(kingpos, MAX_INCREMENT_UL));
    NORMALMOVE(kingpos, max_bidx_inc(kingpos, MAX_INCREMENT_DR));
    NORMALMOVE(kingpos, max_bidx_inc(kingpos, MAX_INCREMENT_DL));
    
    max_plyplate_t plate = board->stack[board->ply];
    if(plate & max_kcastle_flag(board->ply)) {
        max_piececode_t color = MAX_PIECECODE_WHITE << side;

        max_bidx_t between[3] = {
            max_bidx_inc(kingpos, MAX_INCREMENT_RIGHT),
            max_bidx_inc(kingpos, MAX_INCREMENT_RIGHT * 2),
            max_bidx_inc(kingpos, MAX_INCREMENT_RIGHT * 3),
        };
                if(board->pieces[between[0]] == MAX_PIECECODE_EMPTY && board->pieces[between[1]] == MAX_PIECECODE_EMPTY && board->pieces[between[2]] == (MAX_PIECECODE_ROOK | color)) {
            max_movelist_add(moves, max_move_new(kingpos, between[1], MAX_MOVE_KCASTLE));
        }
    }
 
    if(plate & max_qcastle_flag(board->ply)) {
        max_piececode_t color = MAX_PIECECODE_WHITE << side;
        max_bidx_t between[4] = {
            max_bidx_inc(kingpos, MAX_INCREMENT_LEFT),
            max_bidx_inc(kingpos, MAX_INCREMENT_LEFT * 2),
            max_bidx_inc(kingpos, MAX_INCREMENT_LEFT * 3),
            max_bidx_inc(kingpos, MAX_INCREMENT_LEFT * 4)
        };

        if(
            board->pieces[between[0]] == MAX_PIECECODE_EMPTY &&
            board->pieces[between[1]] == MAX_PIECECODE_EMPTY &&
            board->pieces[between[2]] == MAX_PIECECODE_EMPTY &&
            board->pieces[between[3]] == (MAX_PIECECODE_ROOK | color)
        ) {
            max_movelist_add(moves, max_move_new(kingpos, between[2], MAX_MOVE_QCASTLE));
        }
    }
}
