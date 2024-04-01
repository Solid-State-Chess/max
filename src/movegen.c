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

    for(max_lidx_t i = 0; i < state->piecelist.pawn_count; ++i) {
        max_bidx_t pos = state->piecelist.pawns[i];
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
}
