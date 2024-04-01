#include "max.h"
#include "max/board.h"
#include "max/move.h"
#include "max/piece.h"
#include "max/square.h"
#include <stdbool.h>

void max_movegen(max_movelist_t *const moves, max_board_t *const board) {
    //Homerow lookup table, indexed by the current side to move
    static max_bidx_t PAWN_HOMEROW[2]  = {MAX_RANK_2, MAX_RANK_7};
    static max_increment_t PAWN_INC[2] = {MAX_INCREMENT_UP, MAX_INCREMENT_DOWN};
    
    uint8_t side = board->ply & 1;
    max_sidestate_t *state = &board->sides[side];
    max_sidestate_t *enemy = &board->sides[side ^ 1];

    max_bidx_t pawn_homerow = PAWN_HOMEROW[side];
    max_piececode_t enemy_color = MAX_PIECECODE_BLACK >> side;

    for(max_lidx_t i = 0; i < state->piecelist.pawns.len; ++i) {
        max_bidx_t pos = state->piecelist.pawns.pos[i];
        max_bidx_t up = max_bidx_inc(pos, PAWN_INC[side]);

        if(max_bidx_valid(up)) {
            if(board->pieces[up.bits] == MAX_PIECECODE_EMPTY) {
                max_movelist_add(moves, max_move_normal(pos, up));
            }

            max_bidx_t right = max_bidx_inc(up, MAX_INCREMENT_RIGHT);
            if(max_bidx_valid(right) && (board->pieces[right.bits] & enemy_color)) {
                max_movelist_add(moves, max_move_capture(pos, right));
            }

            max_bidx_t left = max_bidx_inc(up, MAX_INCREMENT_LEFT);
            if(max_bidx_valid(left) && (board->pieces[left.bits] & enemy_color)) {
                max_movelist_add(moves, max_move_capture(pos, left));
            }
        }

    }

    #define NORMALMOVE(to) do {     \
        max_bidx_t dest = (to);     \
        if(max_bidx_valid(dest)) {  \
            max_piececode_t piece = board->pieces[dest.bits]; \
            if(piece == MAX_PIECECODE_EMPTY) {                \
                max_movelist_add(moves, max_move_normal(pos, dest)); \
            } else if(piece & enemy_color) { \
                max_movelist_add(moves, max_move_capture(pos, dest)); \
            } \
        } \
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
}
