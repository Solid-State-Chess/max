#include "max/board.h"
#include "max/def.h"
#include "max/move.h"
#include "max/piece.h"
#include "max/square.h"

max_increment_t MAX_DIRECTION_BY_DIFF[240] = {0x3E};

MAX_HOT
bool max_board_move_is_valid(max_board_t *const board, max_move_t move) {
    static max_move_t BUFFER[256] = {0};

    max_piececode_t piece = board->pieces[move.from];
    max_piececode_t color = piece & MAX_PIECECODE_COLOR_MASK;
    
    max_bpos_t kpos = max_board_get_king_pos(board);


    if(move.from == kpos) {
        switch(move.attr) {
            case MAX_MOVE_KCASTLE: {
                return !max_check_exists(board->check) && max_board_attacks(board, max_bpos_inc(kpos, MAX_INCREMENT_RIGHT), NULL) == 0 && max_board_attacks(board, move.to, NULL) == 0;
            } break;

            case MAX_MOVE_QCASTLE: {
                return !max_check_exists(board->check) && max_board_attacks(board, max_bpos_inc(kpos, MAX_INCREMENT_LEFT), NULL) == 0 && max_board_attacks(board, move.to, NULL) == 0;
            } break;

            default: {
                if(max_check_exists(board->check) && max_checker_is_sliding(board->check.attacks[0])) {
                    max_line_t line = board->check.attacks[0].attack.ray;
                    
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
        if(max_check_exists(board->check)) {
            //Only king moves allowed in double check
            if(max_check_is_double(board->check)) {
                return false;
            }

            if(max_checker_is_sliding(board->check.attacks[0])) {
                max_line_t check_sliding = board->check.attacks[0].attack.ray;

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
                if(move.to != board->check.attacks[0].attack.jump) {
                    return false;
                }
            }
        }

        if(move.attr == MAX_MOVE_EN_PASSANT) {
            max_bpos_t captured = max_bpos_inc(move.to, -max_board_get_enemy_pawn_advance_dir(board));

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
            } else if(captured_line == 15 || captured_line == 17 || captured_line == -15 || captured_line == -17) {
                if(max_board_is_empty_between(board, captured, kpos)) {
                }
            }
        }


    check_pin:

        return !max_board_move_exits_pin(
            board,
            piece,
            move.from,
            move.to
        );
    }
}
