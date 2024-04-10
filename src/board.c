#include "max/board.h"
#include "max/piece.h"
#include "max/square.h"


MAX_HOT
bool max_board_get_sliding_attack(
    max_board_t *board,
    max_bpos_t attacked,
    max_piececode_t typemask,
    max_increment_t direction,
    max_line_t *attack
) {
    max_piececode_t piece = board->pieces[attacked];
    max_piececode_t color = max_board_friendly_colormask(board);
    
    max_bpos_t pos = attacked;
    for(;;) {
        pos = max_bpos_inc(pos, direction);
        if(max_bpos_valid(pos)) {
            max_piececode_t attacker = board->pieces[pos];
            if(attacker != MAX_PIECECODE_EMPTY) {
                if(
                    (attacker & color) == 0 &&
                    (attacker & typemask) != 0
                ) {
                    attack->line = -direction;
                    attack->origin = pos;
                    return true;
                }
                
                return false;
            }
        } else {
            return false;
        }
    }
}

MAX_HOT
bool max_board_is_pinned(max_board_t *const board, max_bpos_t from) {
    max_bpos_t king = max_board_get_king_pos(board);
    max_bpos_t diff = max_bpos_diff(king, from);

    max_increment_t line = MAX_DIRECTION_BY_DIFF[diff];
    if(line == 0) {
        return false;
    }

    max_piececode_t friendly = max_board_friendly_colormask(board);

    max_bpos_t pos = from;
    
    if(!max_board_is_empty_between(board, from, king)) {
        return false;
    }


    pos = from;
    do {
        pos = max_bpos_inc(pos, line);
    } while(max_bpos_valid(pos) && (board->pieces[pos] == MAX_PIECECODE_EMPTY));

    if(!max_bpos_valid(pos)) {
        return false;
    }

    max_piececode_t candidate = board->pieces[pos]; 

    if((candidate & friendly) != 0) {
        return false;
    }

    if((candidate & max_get_piece_mask_attacks_direction(line)) != 0) {
        return true;
    } else {
        return false;
    }
}

MAX_HOT
bool max_board_move_exits_pin(
    max_board_t *const board,
    max_piececode_t piece,
    max_bpos_t from,
    max_bpos_t to
) {
    max_bpos_t king = max_board_king_pos(board);

    max_bpos_t diff = max_bpos_diff(king, from);
    max_bpos_t after_diff = max_bpos_diff(king, to);

    max_increment_t line = MAX_DIRECTION_BY_DIFF[diff];

    //If the move does not exit the pin line, we're good
    if(line == MAX_DIRECTION_BY_DIFF[after_diff]) {
        return false;
    }

    return max_board_is_pinned(board, from);
}

MAX_HOT
uint8_t max_board_attacks(max_board_t *const board, max_bpos_t pos, max_checker_t *attacks) {
    max_line_t line;
    uint8_t count = 0;
    max_piececode_t color = max_board_friendly_colormask(board);
    

    static const max_piececode_t SLIDERMASK[2] = {
        MAX_PIECECODE_DIAGONAL_MASK,
        MAX_PIECECODE_CARDINAL_MASK,
    };

    static const max_increment_t *DIRECTIONS[2] = {
        MAX_DIAGONALS,
        MAX_CARDINALS
    };
    
    for(unsigned i = 0; i < 2; ++i) {
        max_increment_t const *direction = DIRECTIONS[i];
        max_piececode_t mask = SLIDERMASK[i];

        for(unsigned j = 0; j < 4; ++j) {
            if(
                max_board_get_sliding_attack(
                    board,
                    pos,
                    mask,
                    direction[j],
                    &line
                )
            ) {
                if(attacks != NULL) {
                    attacks += 1;
                }
                count += 1;
            }
        }
    }
    
    static const max_piececode_t JUMPMASK[3] = {
        MAX_PIECECODE_PAWN,
        MAX_PIECECODE_KNIGHT,
        MAX_PIECECODE_KING
    };

    max_increment_t pawn_attackers[2];
    pawn_attackers[0] = MAX_PAWNSIDES[0] - max_board_get_enemy_pawn_advance_dir(board);
    pawn_attackers[1] = MAX_PAWNSIDES[1] - max_board_get_enemy_pawn_advance_dir(board);
    
    /// Offsets to check for knight, pawn and king moves
    max_increment_t const *offsets[3] = {
        pawn_attackers,
        MAX_KNIGHT_MOVES,
        MAX_KING_MOVES,
    };

    uint8_t offset_lens[3] = {2, MAX_KNIGHT_MOVES_LEN, MAX_KING_MOVES_LEN};
    
    for(unsigned i = 0; i < 3; ++i) {
        max_increment_t const *offset = offsets[i];
        uint8_t offsets_len = offset_lens[i];
        max_piececode_t mask = JUMPMASK[i];

        for(unsigned j = 0; j < offsets_len; ++j) {
            max_bpos_t attacker = max_bpos_inc(pos, offset[j]);
            if(
                max_bpos_valid(attacker) &&
                (board->pieces[attacker] & color) == 0 &&
                (board->pieces[attacker] & mask)
            ) {
                if(attacks != NULL) {
                    attacks->attack.jump = attacker;
                    attacks += 1;
                }
                count += 1;
            }
        }
    }
    
    return count;
}
