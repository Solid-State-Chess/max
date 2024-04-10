#include "max/board.h"
#include "max/piece.h"
#include "max/square.h"
#include "private.h"

void max_board_update_check(max_board_t *const board, max_bpos_t from, max_bpos_t to) {
    max_check_reset(&board->check);

    max_checker_t *check = &board->check.attacks[0];

    max_bpos_t kpos = max_board_get_king_pos(board);
    max_piececode_t friendly = max_board_friendly_colormask(board);
    
    //Search for a discovered attack by a sliding piece hidden behind the piece
    max_increment_t discovered_line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(from, kpos)];
    if(discovered_line != 0) {
        max_bpos_t search_attacker = from;
        do {
            search_attacker = max_bpos_inc(search_attacker, -discovered_line);
        } while(max_bpos_valid(search_attacker) && board->pieces[search_attacker] == MAX_PIECECODE_EMPTY);

        if(max_bpos_valid(search_attacker)) {
            max_piececode_t possible_attacker = max_get_piece_mask_attacks_direction(discovered_line);
            max_piececode_t attacker = board->pieces[search_attacker];

            if((attacker & possible_attacker) && (attacker & friendly) == 0) {
                if(max_board_is_empty_between(board, to, kpos)) {
                    check->attack.ray = (max_line_t) {
                        .origin = to,
                        .line = discovered_line
                    };
                    check += 1;
                }
            }
        }
    }
}
