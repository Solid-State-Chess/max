#include "max/board/board.h"
#include "max/board/dir.h"
#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/piececode.h"
#include "max/def.h"
#include "private/board/board.h"
#include <stdio.h>

/// Check if a piece on the given square is pinned by an enemy slider to the friendly king.
/// \return The pinning direction from king to `pos` if the piece is pinned, otherwise #MAX_0x88_DIR_INVALID
static MAX_INLINE_ALWAYS max_0x88_dir_t max_board_piece_is_pinned(max_board_t *board, max_0x88_t pos) {
    max_0x88_t kpos = *max_board_side_list(board, max_board_side(board))->king.loc;
    
    max_0x88_dir_t dir = max_0x88_line(kpos, pos);
    if(dir != MAX_0x88_DIR_INVALID) {
        max_piecemask_t mask = max_0x88_piecemask_for_dir(dir);
        max_piecemask_t enemy = max_side_color_mask(max_board_enemy_side(board));
        
        //Move along the king->piece line to check if there is a valid slider behind the pinned piece
        max_0x88_t scan = pos;
        for(;;) {
            scan = max_0x88_move(scan, dir);
            if(!max_0x88_valid(scan)) {
                return MAX_0x88_DIR_INVALID;
            }

            max_piececode_t attacker = board->pieces[scan.v];
            if(attacker.v != MAX_PIECECODE_EMPTY) {
                if(max_piececode_match(attacker, mask) && max_piececode_match(attacker, enemy)) {
                    break;
                }
                return MAX_0x88_DIR_INVALID;
            }
        }
        
        //If there is an enemy slider on the given line and the line between the piece and the king is empty, then the piece is pinned
        if(max_board_empty_between_with_dir(board, kpos, pos, dir)) {
            return dir;
        } else {
            return MAX_0x88_DIR_INVALID;
        }
    } else {
        return MAX_0x88_DIR_INVALID;
    }
}

bool max_board_legal(max_board_t *board, max_smove_t move) {
    static max_smove_t buf[512];
    max_state_t *state = max_board_state(board);

    max_piececode_t moved = board->pieces[move.from.v];
    if((moved.v & MAX_PIECECODE_TYPE_MASK) == MAX_PIECECODE_KING) {
        if(move.tag == MAX_MOVETAG_ACASTLE || move.tag == MAX_MOVETAG_HCASTLE) {

        } else {
            return !max_board_square_is_attacked(board, move.to);
        }
    }

    if(!max_check_is_empty(state->check[0])) {
        //Only king moves are allowed when in double check
        if(!max_check_is_empty(state->check[1])) {
            return false;
        }

        max_check_t check = state->check[0];
        if(max_check_is_sliding(check)) {
            max_0x88_t kpos = *max_board_side_list(board, max_board_side(board))->king.loc;
            max_0x88_dir_t dir = max_0x88_line(kpos, move.to);
            return (dir == check.ray) && (max_0x88_diff(kpos, check.origin).v > max_0x88_diff(kpos, move.to).v);
        } else {
            return move.to.v == check.origin.v;
        }
    } else {
        max_0x88_dir_t pin_dir = max_board_piece_is_pinned(board, move.from);
        if(pin_dir != MAX_0x88_DIR_INVALID) {
            printf("PINNED @ %d\n", pin_dir);
            max_0x88_t kpos = *max_board_side_list(board, max_board_side(board))->king.loc;
            return pin_dir != max_0x88_line(kpos, move.to);
        } else {
            return true;
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
