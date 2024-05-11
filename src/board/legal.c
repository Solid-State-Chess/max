#include "max/board/board.h"
#include "max/board/dir.h"
#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/piececode.h"
#include "max/board/state.h"
#include "max/def.h"
#include "private/board/board.h"
#include "private/board/movegen/king.h"
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
        if((move.tag & ~MAX_MOVETAG_CAPTURE) != MAX_MOVETAG_NONE) {
            max_side_t side = max_board_side(board);
            MAX_SANITY(
                max_movetag_is_castle(move.tag) &&
                "King makes a move that is not a capture or castle"
            );

            max_castle_side_t castle_side = max_castle_side_for_movetag(move.tag);
            MAX_SANITY(
                (state->packed & max_packed_state_castle(side, castle_side)) != 0 &&
                "Castle move played when the side does not have castle rights"
            );

            max_0x88_t scan = move.from;
            max_0x88_t dest = MAX_CASTLE_KING_DEST[castle_side][side];

            max_0x88_dir_t dir = max_0x88_line(scan, dest);
            MAX_SANITY(dir != 0 && "King does not have a line to its own destination square");
             
            for(;;) {
                //We don't have to see if king is in check because castle moves are not generated
                //in the pseudolegal generator if the king is in check
                scan = max_0x88_move(scan, dir);
                if(max_board_square_is_attacked(board, scan)) {
                    return false;
                }

                if(scan.v == dest.v) {
                    break;
                }
            }

            return true;
        } else {

            for(uint8_t i = 0; i < 2; ++i) {
                if(
                    max_check_is_sliding(state->check[i]) &&
                    max_0x88_line(move.from, state->check[i].origin) == state->check[i].ray
                ) {
                    return false;
                }
            }

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
            if(dir != check.ray) {
                return false;
            }
            
            max_0x88_t scan = kpos;
            for(;;) {
                scan = max_0x88_move(scan, dir);
                if(scan.v == move.to.v) {
                    return true;
                } else if(scan.v == check.origin.v) {
                    return false;
                }
            }
        } else {
            return move.to.v == check.origin.v;
        }
    } else {
        //Kind of a hack: put the pawn captured en passant on time out while we check for pins
        max_0x88_t ep_square;
        max_piececode_t ep_piece;
        if(move.tag == MAX_MOVETAG_ENPASSANT) {
            MAX_SANITY(max_packed_state_epfile(state->packed) != MAX_PSTATE_EPFILE_INVALID);
            ep_square = max_0x88_new(
                max_0x88_rank(move.from),
                max_packed_state_epfile(state->packed)
            );
            ep_piece = board->pieces[ep_square.v];
            board->pieces[ep_square.v].v = MAX_PIECECODE_EMPTY;
        }

        max_0x88_dir_t pin_dir = max_board_piece_is_pinned(board, move.from);

        if(move.tag == MAX_MOVETAG_ENPASSANT) {
            board->pieces[ep_square.v] = ep_piece;
        }

        if(pin_dir != MAX_0x88_DIR_INVALID) {
            max_0x88_t kpos = *max_board_side_list(board, max_board_side(board))->king.loc;
            return pin_dir == max_0x88_line(kpos, move.to);
        } else {
            return true;
        }
    }
}
