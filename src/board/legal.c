#include "max/board/board.h"
#include "max/board/loc.h"
#include "max/board/movegen.h"
#include "private/board/board.h"

bool max_board_legal(max_board_t *board, max_smove_t move) {
    static max_smove_t buf[512];
    max_state_t *state = max_board_state(board);

    max_piececode_t moved = board->pieces[move.from.v];
    if(!max_check_is_empty(state->check[0])) {
        //Only king moves are allowed when in double check
        if(!max_check_is_empty(state->check[1])) {
            if(!max_piececode_match(moved, max_piecemask_new(MAX_PIECECODE_KING))) {
                return false;
            }
        }

        if(max_piececode_match(moved, max_piecemask_new(MAX_PIECECODE_KING))) {

        } else {
            max_check_t check = state->check[0];
            if(max_check_is_sliding(check)) {
                max_0x88_t kpos = *max_board_side_list(board, max_board_side(board))->king.loc;
                max_0x88_dir_t dir = max_0x88_line(kpos, move.to);
                return (dir == check.ray) && (max_0x88_diff(kpos, check.origin).v > max_0x88_diff(kpos, move.to).v);
            } else {
                return move.to.v == check.origin.v;
            }
        }
    }

    max_movelist_t moves;
    max_movelist_new(&moves, buf, 512);
    max_board_make_move(board, move);

    max_board_movegen(board, &moves);
    for(unsigned i = 0; i < moves.len; ++i) {
        if(moves.buf[i].to.v == max_board_side_list(board, max_board_enemy_side(board))->king.loc->v) {
            max_board_unmake_move(board, move);
            return false;
        }
    }
    
    max_board_unmake_move(board, move);
    return true;
}
