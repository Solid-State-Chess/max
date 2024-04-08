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
    
    max_bpos_t kps = max_board_get_king_pos(board);
    max_bpos_t kpos = board->sides[color >> MAX_PIECECODE_BLACK_OFFSET].king.pos[0];


    if(move.from == kpos) {
        switch(move.attr) {
            case MAX_MOVE_KCASTLE: {
                return !max_board_attacked(board, kpos) && !max_board_attacked(board, max_bpos_inc(kpos, MAX_INCREMENT_RIGHT)) && !max_board_attacked(board, move.to);
            } break;

            case MAX_MOVE_QCASTLE: {
                return !max_board_attacked(board, kpos) && !max_board_attacked(board, max_bpos_inc(kpos, MAX_INCREMENT_LEFT)) && !max_board_attacked(board, move.to);
            } break;

            default: {
                if(max_check_exists(board->check) && max_checker_is_sliding(board->check.attacks[0])) {
                    max_line_t line = board->check.attacks[0].attack.ray;
                    
                    max_increment_t after_line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(line.origin, move.to)];
                    if(after_line == line.line) {
                        return false;
                    }
                }
                
                return !max_board_attacked(board, move.to);
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
            max_bpos_t captured = max_bpos_inc(move.to, max_board_get_enemy_pawn_advance_dir(board));

            max_increment_t captured_line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(captured, kpos)];
            if(captured_line == 0) {
                goto check_pin;
            }

            max_increment_t post_move_line = MAX_DIRECTION_BY_DIFF[max_bpos_diff(move.to, kpos)];
            
            //En passant may result in discovered attack on the king, check for this
            if(captured_line != post_move_line) {
                max_bpos_t pos = captured;
                do {
                    pos = max_bpos_inc(pos, captured_line);
                } while(board->pieces[pos] == MAX_PIECECODE_EMPTY);
                
                //There is another blocker on the line to the king
                if(pos != kpos) {
                    goto check_pin;
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
