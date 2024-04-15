#include "max/board/board.h"
#include "max/board/op.h"
#include "max/def.h"
#include "max/board/move.h"
#include "max/board/piece.h"
#include "max/board/square.h"
#include "../private.h"

/// Check if the piece on the given `pos` delivers check to the given `kpos`
MAX_INLINE_ALWAYS static
bool max_board_piece_delivers_check(max_board_t *const board, max_bpos_t pos, max_bpos_t kpos, max_checker_t *checker) {
    max_piececode_t piece = board->pieces[pos];
    max_piececode_t color = piece & MAX_PIECECODE_COLOR_MASK;

    if((color & max_board_friendly_colormask(board)) != 0) {
        return false;
    }

    switch(piece & MAX_PIECECODE_TYPE_MASK) {
        case MAX_PIECECODE_PAWN: {
            max_bpos_t down = max_bpos_inc(
                pos,
                max_board_enemy_pawn_advance_dir(board)
            );

            if(max_bpos_inc(down, MAX_INCREMENT_LEFT)  == kpos || max_bpos_inc(down, MAX_INCREMENT_RIGHT) == kpos) {
                checker->attack.jump = pos;
                return true;
            }
        } break;

        case MAX_PIECECODE_KNIGHT: {
            max_bpos_t diff = max_bpos_diff(pos, kpos);
            for(unsigned i = 0; i < MAX_KNIGHT_MOVES_LEN; ++i) {
                if(diff == (0x77 + MAX_KNIGHT_MOVES[i])) {
                    checker->attack.jump = pos;
                    return true;
                }
            }
        } break;

        case MAX_PIECECODE_EMPTY: {

        } break;

        default: {
            if(max_board_is_empty_between(board, pos, kpos)) {
                max_increment_t line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(pos, kpos)];
                max_piececode_t mask = max_get_piece_mask_attacks_direction(line);
                if((piece & mask) != 0) {
                    checker->attack.ray = (max_line_t) {
                        .line = line,
                        .origin = pos
                    };
                    return true;
                }
            } else {
            }
        } break;
    }

    return false;
}


bool max_board_update_discovery(max_board_t *const board, max_bpos_t empty, max_checker_t *check) {
    max_bpos_t kpos = max_board_friendly_king_pos(board);
    max_piececode_t friendly = max_board_friendly_colormask(board);


    //Search for a discovered attack by a sliding piece hidden behind the piece
    max_increment_t discovered_line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(empty, kpos)];
    if(discovered_line != 0) {
        max_bpos_t search_attacker = empty;
        do {
            search_attacker = max_bpos_inc(search_attacker, -discovered_line);
        } while(max_bpos_valid(search_attacker) && board->pieces[search_attacker] == MAX_PIECECODE_EMPTY);
        
        if(max_bpos_valid(search_attacker)) {
            max_piececode_t possible_attacker = max_get_piece_mask_attacks_direction(discovered_line);
            max_piececode_t attacker = board->pieces[search_attacker];
            
            if((attacker & possible_attacker) != 0 && (attacker & friendly) == 0) {
                if(max_board_is_empty_between(board, empty, kpos)) {
                    check->attack.ray = (max_line_t) {
                        .origin = search_attacker,
                        .line = discovered_line
                    };
                    return true;
                }
            }
        }
    }
    
    return false;
}


void max_board_update_check(max_board_t *const board, max_move_t move, max_check_t *checks) {
    max_check_reset(checks);
    max_checker_t *check = &checks->attacks[0];

    max_bpos_t kpos = max_board_friendly_king_pos(board);
    
    if(max_board_piece_delivers_check(board, move.to, kpos, check)) {
        check += 1;
    }

    //Search for a discovered attack by a sliding piece hidden behind the piece
    if(max_board_update_discovery(board, move.from, check)) {
        check += 1;
    }
    if(move.attr == MAX_MOVE_EN_PASSANT) {
        max_bpos_t captured = max_bpos_inc(move.to, max_board_enemy_pawn_advance_dir(board));
        if(max_board_update_discovery(board, captured, check)) {
        }
    }
}
