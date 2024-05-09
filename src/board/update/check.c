#include "max/assert.h"
#include "max/board/board.h"
#include "max/board/dir.h"
#include "max/board/piececode.h"
#include "max/board/state.h"
#include "private/board/board.h"
#include "private/board/movegen.h"
#include "private/board/movegen/knight.h"
#include "private/board/movegen/pawn.h"


static max_check_t* max_board_piece_delivers_check(max_board_t *board, max_0x88_t kpos, max_0x88_t pos, max_piececode_t piece, max_check_t *check) {
    MAX_SANITY(board->pieces[pos.v].v == piece.v);

    switch(piece.v & MAX_PIECECODE_TYPE_MASK) {
        case MAX_PIECECODE_PAWN: {
            //Advance the king position to avoid having to negate the advance direction or get the enemy side from the board
            max_0x88_t advanced = max_0x88_move(kpos, MAX_PAWN_ADVANCE_DIR[max_board_side(board)]);
            if(
                max_0x88_move(advanced, MAX_PAWN_ATTACK_SIDES[0]).v == pos.v ||
                max_0x88_move(advanced, MAX_PAWN_ATTACK_SIDES[1]).v == pos.v
            ) {
                check->origin = pos;
                return check + 1;
            }
        } break;

        case MAX_PIECECODE_KNIGHT: {
            for(unsigned i = 0; i < MAX_KNIGHT_MOVES_LEN; ++i) {
                if(max_0x88_move(pos, MAX_KNIGHT_MOVES[i]).v == kpos.v) {
                    check->origin = pos;
                    return check + 1;
                }
            }
        } break;

        default: {
            max_0x88_dir_t dir = max_0x88_line(kpos, pos);
            if(
                max_piececode_match(piece, max_0x88_piecemask_for_dir(dir)) &&
                max_board_empty_between_with_dir(board, kpos, pos, dir)
            ) {
                check->origin = pos;
                check->ray = dir;
                return check + 1;
            }
        } break;
    }

    return check + 1;
}

static max_check_t* max_board_update_discovered_check(max_board_t *board, max_0x88_t kpos, max_0x88_t from, max_check_t *check) {
    max_0x88_dir_t dir = max_0x88_line(kpos, from);
    if(dir != MAX_0x88_DIR_INVALID) {
        if(max_board_empty_between_with_dir(board, kpos, from, dir)) {
            max_0x88_t scan = from;
            for(;;) {
                scan = max_0x88_move(scan, dir);
                if(!max_0x88_valid(scan)) {
                    return check;
                }

                max_piececode_t piece = board->pieces[scan.v];
                if(piece.v != MAX_PIECECODE_EMPTY) {
                    max_piecemask_t mask = max_0x88_piecemask_for_dir(dir);
                    if(max_piececode_match(piece, mask) && !max_piececode_match(piece, max_side_color_mask(max_board_side(board)))) {
                        check->origin = scan;
                        check->ray = dir;
                        return check + 1;
                    }
                    return check;
                }
            }
        }
    }

    return check;
}

void max_board_update_check(max_board_t *board, max_0x88_t from, max_0x88_t to) {
    max_state_t *state = max_board_state(board);
    max_check_t *check = state->check;
    max_0x88_t kpos = *max_board_side_list(board, max_board_side(board))->king.loc;
    
    max_piececode_t piece = board->pieces[to.v];

    //Update the check pointer if the given piece delivers check itself
    check = max_board_piece_delivers_check(board, kpos, to, piece, check);
    check = max_board_update_discovered_check(board, kpos, from, check);
}
