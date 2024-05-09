#include "max/assert.h"
#include "max/board/board.h"
#include "max/board/dir.h"
#include "max/board/piececode.h"
#include "max/board/state.h"
#include "private/board/board.h"
#include "private/board/movegen.h"
#include "private/board/movegen/knight.h"
#include "private/board/movegen/pawn.h"

/// Efficiently see if a piece on the given square attacks the given position `kpos`.
/// If check is detected, the given #max_check_t structure is updated with sliding / jumping data, and an incremented pointer is returned
/// to indicate that the next check method is checking for double checks.
/// \param kpos Position to check for attacks by a piece on the given square
/// \param pos Position of the attacker piece
/// \param [out] check Structure to update with delivered check data if it is detected
/// \return `check + 1` if check was detected, and the value of the `check` parameter otherwise (for use with double check detection)
static max_check_t* max_board_piece_delivers_check(max_board_t *board, max_0x88_t kpos, max_0x88_t pos, max_check_t *check) {
    max_piececode_t piece = board->pieces[pos.v];
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

    return check;
}

/// Update the given check structure with discovered check when a piece has been removed from the given position.
/// If check is detected, the #max_check_t structure pointed to by the `check` parameter is updated with the sliding check data.
/// \param kpos Position to check for discovered sliding attacks on
/// \param from The location that has been left empty by a move
/// \param [out] check Structure to update with sliding discovered check if any is detected
/// \return `check + 1` if sliding discovered attack was found, or the value of the `check` parameter otherwise
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
                    if(max_piececode_match(piece, max_piecemask_combine(mask, max_side_color_mask(max_board_side(board))))) {
                        check->origin = scan;
                        check->ray = dir;
                        return check + 1;
                    }
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

    //Update the check pointer if the given piece delivers check itself
    check = max_board_piece_delivers_check(board, kpos, to, check);
    check = max_board_update_discovered_check(board, kpos, from, check);
}
