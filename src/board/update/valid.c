#include "max/board/board.h"
#include "max/board/op.h"
#include "max/def.h"
#include "max/board/move.h"
#include "max/board/piece.h"
#include "max/board/square.h"

max_increment_t MAX_DIRECTION_BY_DIFF[240] = {0x3E};

MAX_HOT
bool max_board_move_is_valid(max_board_t *const board, max_move_t move) {
    max_piececode_t piece = board->pieces[move.from];
    max_piececode_t color = piece & MAX_PIECECODE_COLOR_MASK;
    
    max_bpos_t kpos = max_board_friendly_king_pos(board);
    max_irreversible_t *state = max_board_state(board);


    if(move.from == kpos) {
        switch(move.attr) {
            case MAX_MOVE_KCASTLE: {
                return !max_check_exists(state->check) &&
                    !max_board_attacks(board, max_bpos_inc(kpos, MAX_INCREMENT_RIGHT), NULL) &&
                    !max_board_attacks(board, move.to, NULL);
            } break;

            case MAX_MOVE_QCASTLE: {
                return !max_check_exists(state->check) &&
                    !max_board_attacks(board, max_bpos_inc(kpos, MAX_INCREMENT_LEFT), NULL) &&
                    !max_board_attacks(board, move.to, NULL);
            } break;

            default: {
                if(max_check_exists(state->check) && max_checker_is_sliding(state->check.attacks[0])) {
                    max_line_t line = state->check.attacks[0].attack.ray;
                    
                    max_increment_t after_line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(line.origin, move.to)];
                    if(after_line == line.line) {
                        return false;
                    }
                }
                
                return max_board_attacks(board, move.to, NULL) == 0;
            } break;
        }
    } else {
        //King in check, need to see if the move blocks check
        if(max_check_exists(state->check)) {
            //Only king moves allowed in double check
            if(max_check_is_double(state->check)) {
                return false;
            }

            if(max_checker_is_sliding(state->check.attacks[0])) {
                max_line_t check_sliding = state->check.attacks[0].attack.ray;

                if(move.to == check_sliding.origin) {
                    goto check_pin;
                }
                
                if(MAX_DIRECTION_BY_DIFF[max_bpos_diff(move.to, kpos)] == check_sliding.line) {
                    max_bpos_t pos = check_sliding.origin;
                    do { pos = max_bpos_inc(pos, check_sliding.line); }
                    while(pos != move.to && board->pieces[pos] == MAX_PIECECODE_EMPTY);
                    
                    //TODO: abs((int8_t)(kpos) - (int8_t)(move.to)) < abs((int8_t)(kpos) - (int8_t)(check_sliding.origin))) {
                    if(pos != kpos) {
                        goto check_pin;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                if(move.to != state->check.attacks[0].attack.jump) {
                    return false;
                }
            }
        }

        if(move.attr == MAX_MOVE_EN_PASSANT) {
            return false;
            max_bpos_t captured = max_bpos_inc(move.to, max_board_enemy_pawn_advance_dir(board));
            
            //FIXME: This will cause en passant to be denied even when the captured piece is vertically pinned
            if(max_board_is_pinned(board, captured)) {
                return false;
            }

            max_increment_t captured_line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(captured, kpos)];
            if(captured_line == 0) {
                goto check_pin;
            }

            max_increment_t pre_move_line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(move.from, kpos)];
            
            //En passant may result in discovered attack on the king, check for this
            //if both pawns lie on the same rank as the king, then horizontal discovered attack may occur
            if(captured_line == pre_move_line) {
                max_bpos_t pos = captured;
                do {
                    pos = max_bpos_inc(pos, captured_line);
                } while(pos != move.from && board->pieces[pos] == MAX_PIECECODE_EMPTY);
                
                //There is no other blocker on the line to the king
                if(pos == kpos) {
                    //No blockers on the line to the king, we can continue to check if there is a rook on the same rank
                    pos = captured;
                    do {
                        pos = max_bpos_inc(pos, -captured_line);
                    } while(max_bpos_valid(pos) && pos != move.from && board->pieces[pos] == MAX_PIECECODE_EMPTY);

                    if(max_bpos_valid(pos)) {
                        max_piececode_t piece = board->pieces[pos];
                        if(
                            (piece & color) == 0 &&
                            (piece & MAX_PIECECODE_ROOK)
                        ) {
                            return false;
                        }
                    }
                }
            }
        }


    check_pin:

        return !max_board_move_exits_pin(
            board,
            move.from,
            move.to
        );
    }
}
