#include "max/board.h"
#include "max/def.h"
#include "max/move.h"
#include "max/piece.h"
#include "max/square.h"
#include <stdlib.h>

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
                return !max_board_attacked(board, kpos) && !max_board_attacked(board, max_bpos_inc(kpos, MAX_INCREMENT_RIGHT));
            } break;

            case MAX_MOVE_QCASTLE: {
                return !max_board_attacked(board, kpos) && !max_board_attacked(board, max_bpos_inc(kpos, MAX_INCREMENT_LEFT));
            } break;

            default: {
                return !max_board_attacked(board, move.to);
            } break;
        }
    } else if(true || move.attr == MAX_MOVE_EN_PASSANT) {
        max_movelist_t moves = max_movelist_new(BUFFER);
        
        max_board_make_move(board, move);

        kpos = board->sides[(board->ply & 1) ^ 1].king.pos[0];
        
        max_board_movegen_pseudo(board, &moves);

        for(unsigned i = 0; i < moves.len; ++i) {
            if(moves.moves[i].to == kpos) {
                max_board_unmake_move(board, move);
                return false;
            }
        }
        
        max_board_unmake_move(board, move);
        return true;
    } else if(false) {
        //King in check, need to see if the move blocks check
        if(max_check_exists(board->check)) {
            if(max_checker_is_sliding(board->check.attacks[0])) {
                max_line_t check_sliding = board->check.attacks[0].attack.ray;

                if(move.to == check_sliding.origin) {
                    goto check_pin;
                }
                
                if(MAX_DIRECTION_BY_DIFF[max_bpos_diff(move.to, kpos)] == check_sliding.line) {
                    //puts("SAMELINE");
                    max_bpos_t pos = check_sliding.origin;
                    do { pos = max_bpos_inc(pos, check_sliding.line); }
                    while(pos != move.to && board->pieces[pos] == MAX_PIECECODE_EMPTY);

                    if(abs((int8_t)(kpos) - (int8_t)(move.to)) < abs((int8_t)(kpos) - (int8_t)(check_sliding.origin))) {
                        goto check_pin;
                    } else {
                        return false;
                    }
                        
                    //We jumped either between the attacker or behind it, check
                    /*if(pos == kpos) {
                        return false;
                    }*/
                } else {
                    return false;
                }
            } else {
                if(move.to != board->check.attacks[0].attack.jump) {
                    return false;
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
