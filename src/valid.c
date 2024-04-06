#include "max/board.h"
#include "max/def.h"
#include "max/piece.h"
#include "max/square.h"
#include "private.h"
#include <stdlib.h>

max_increment_t MAX_DIRECTION_BY_DIFF[140] = {0x3E};

static MAX_INLINE_ALWAYS int8_t sign(int8_t n) {
    return (n & 0b10000000) ? -1 : 1;
}


void max_board_init_statics(max_board_t *const board) {
    for(int8_t x = 0; x < 8; ++x) {
        for(int8_t y = 0; y < 8; ++y) {
            max_bpos_t from = max_bpos_new(x, y);

            for(int8_t to_x = 0; to_x < 8; ++to_x) {
                for(int8_t to_y = 0; to_y < 8; ++to_y) {
                    max_bpos_t to = max_bpos_new(to_x, to_y);

                    int8_t xd = to_x - x;
                    int8_t yd = to_y - y;

                    max_increment_t direction = 0;
                    if(x == to_x) {
                        direction = 0x10 * sign(yd);
                    } else if(y == to_y) {
                        direction = 0x01 * sign(xd); 
                    }

                    if(abs(xd) == abs(yd)) {
                        direction = (0x10 * sign(yd)) + (0x01 * sign(xd));
                    }

                    MAX_DIRECTION_BY_DIFF[max_bpos_diff(from, to)] = direction;
                }
            }
        }
    }
}

/// Information for a detected sliding attack on a square
typedef struct {
    max_increment_t line;
    max_bpos_t attacker;
} max_lineattack_t;

/// Check for a sliding attacker on the given square, returns true if there is a sliding attack on square
MAX_HOT
MAX_INLINE_ALWAYS
bool max_board_sliding_attack(max_board_t *const board, max_bpos_t square, max_piececode_t piece, max_lineattack_t *attack) {
    static const max_increment_t  DIAGONAL[4] = {
        MAX_INCREMENT_UR,
        MAX_INCREMENT_UL,
        MAX_INCREMENT_DR,
        MAX_INCREMENT_DL,
    };

    static const max_increment_t CARDINAL[4] = {
        MAX_INCREMENT_UP,
        MAX_INCREMENT_LEFT,
        MAX_INCREMENT_RIGHT,
        MAX_INCREMENT_DOWN
    };

    max_piececode_t color = piece & MAX_PIECECODE_COLOR_MASK;

    for(unsigned i = 0; i < 4; ++i) {
        max_increment_t dir = DIAGONAL[i];
        max_bpos_t pos = square;
        do {
            pos = max_bpos_inc(pos, dir);
        } while(max_bpos_valid(pos) && board->pieces[pos] == MAX_PIECECODE_EMPTY);

        if(max_bpos_valid(pos) && (board->pieces[pos] & MAX_PIECECODE_BISHOP) && (board->pieces[pos] & color) == 0) {
            attack->line = dir;
            attack->attacker = pos;
            return true;
        }
    }

    for(unsigned i = 0; i < 4; ++i) {
        max_increment_t dir = CARDINAL[i];
        max_bpos_t pos = square;
        do {
            pos = max_bpos_inc(pos, dir);
        } while(max_bpos_valid(pos) && board->pieces[pos] == MAX_PIECECODE_EMPTY);

        if(max_bpos_valid(pos) && (board->pieces[pos] & MAX_PIECECODE_ROOK) && (board->pieces[pos] & color) == 0) {
            attack->line = dir;
            attack->attacker = pos;
            return true;
        }
    }

    return false;
}

/// Check if there is a knight or pawn attack on the given square
MAX_HOT
MAX_INLINE_ALWAYS
bool max_board_nonsliding_attack(max_board_t *board, max_bpos_t attacked, max_piececode_t piece, max_bpos_t *attacker) {
    static const max_increment_t PAWNSIDES[2] = {MAX_INCREMENT_RIGHT, MAX_INCREMENT_LEFT};
    
    max_piececode_t color = piece & MAX_PIECECODE_COLOR_MASK;

    for(unsigned i = 0; i < 8; ++i) {
        max_bpos_t pos = max_bpos_inc(attacked, KNIGHT_MOVES[i]);
        if(
            max_bpos_valid(pos) &&
            (board->pieces[pos] & color) == 0 &&
            (board->pieces[pos] & MAX_PIECECODE_TYPE_MASK) == MAX_PIECECODE_KNIGHT
        ) {
            *attacker = pos;
            return true;
        }
    }

    for(uint8_t i = 0; i < 2; ++i) {
        max_bpos_t pos = max_bpos_inc(attacked, PAWNSIDES[i] + PAWN_INC[color >> MAX_PIECECODE_BLACK_OFFSET]);
        if(
            max_bpos_valid(pos) &&
            (board->pieces[pos] & color) == 0 &&
            (board->pieces[pos] & MAX_PIECECODE_TYPE_MASK) == MAX_PIECECODE_PAWN
        ) {
            *attacker = pos;
            return true;
        }
    }

    return false;
}

MAX_HOT
MAX_INLINE_ALWAYS
bool max_board_move_exits_pin(
    max_board_t *const board,
    max_piececode_t piece,
    max_bpos_t from,
    max_bpos_t to,
    max_bpos_t king
) {
    /// Indexed by 0 = 0 in rank OR file difference, 1 = both rank and file difference(diagonal)
    static const max_piececode_t PINNER_BY_DIR[2] = {
        MAX_PIECECODE_ROOK,
        MAX_PIECECODE_BISHOP
    };

    max_bpos_t diff = max_bpos_diff(king, from);
    max_bpos_t after_diff = max_bpos_diff(king, to);

    max_increment_t line = MAX_DIRECTION_BY_DIFF[diff];
    if(line == 0) {
        return false;
    }
    

    //If the move does not exit the pin line, we're good
    if(line == MAX_DIRECTION_BY_DIFF[after_diff]) {
        return false;
    }

    //Check that there are no pieces on the line between piece and king
    max_increment_t opposite = -line;
    max_bpos_t pos = from;
    //printf("King on %X, piece at %X, inc is %d\n", king, from, -line);
    for(;;) {
        pos = max_bpos_inc(pos, opposite); 
        if(pos == king) { break; }
        if(board->pieces[pos] != MAX_PIECECODE_EMPTY) {
            return false;
        }
    }


    pos = max_bpos_inc(from, line);
    while(max_bpos_valid(pos) && (board->pieces[pos] == MAX_PIECECODE_EMPTY)) {
        pos = max_bpos_inc(pos, line);
    }

    if(!max_bpos_valid(pos)) {
        return false;
    }

    max_piececode_t candidate = board->pieces[pos];
    uint8_t is_diagonal = ((king & 0x0F) != (from & 0x0F) && (king & 0xF0) != (from & 0xF0));
    max_piececode_t possible_pinner = PINNER_BY_DIR[is_diagonal];

    if(
        (candidate & possible_pinner) &&
        (board->pieces[pos] & (piece & MAX_PIECECODE_COLOR_MASK)) == 0
    ) {
        return true;
    }
    
    return false;
}


MAX_HOT
bool max_board_move_is_valid(max_board_t *const board, max_move_t move) {
    static max_move_t BUFFER[256] = {0};

    max_piececode_t piece = board->pieces[move.from];
    max_piececode_t color = piece & MAX_PIECECODE_COLOR_MASK;
    
    bool known_good = true;
    if(true) {// || piece == MAX_PIECECODE_KING || move.attr == MAX_MOVE_EN_PASSANT) {
        max_movelist_t moves = max_movelist_new(BUFFER);
        
        max_board_make_move(board, move);

        max_bpos_t kpos = board->sides[(board->ply & 1) ^ 1].king.pos[0];
        
        max_board_movegen_pseudo(board, &moves);

        for(unsigned i = 0; i < moves.len; ++i) {
            if(moves.moves[i].to == kpos) {
                max_board_unmake_move(board, move);
                    return false;
                //known_good = false;
                //break;
            }        
        }
        
        max_board_unmake_move(board, move);
        return true;
    
    }/* else {

        bool pincheck = false;
        //if((piece & MAX_PIECECODE_TYPE_MASK) != MAX_PIECECODE_KING) {
            max_lineattack_t check_sliding;
            max_bpos_t attacker;

            //King in check, need to see if the move blocks check
            if(max_board_sliding_attack(board, kpos, piece, &check_sliding)) {
                if(move.to == check_sliding.attacker) {
                    pincheck = true; 
                } else {
                    if(-MAX_DIRECTION_BY_DIFF[max_bpos_diff(move.to, kpos)] == check_sliding.line) {
                        /*max_bpos_t pos = kpos;
                        do { pos = max_bpos_inc(pos, check_sliding.line); }
                        while(board->pieces[pos] == MAX_PIECECODE_EMPTY);
                            
                        //We jumped either between the attacker or behind it, check
                        pincheck = abs((int8_t)(kpos) - (int8_t)(move.to)) < abs((int8_t)(kpos) - (int8_t)(check_sliding.attacker)); //(board->pieces[pos] & color) == 0;
                    }
                }
            } else if(max_board_nonsliding_attack(board, kpos, piece, &attacker)) {
                pincheck = move.to == attacker;
            } else {
                pincheck = !max_board_move_exits_pin(
                    board,
                    piece,
                    move.from,
                    move.to,
                    kpos
                );
            }

            return pincheck;




        /*if(pincheck != known_good) {
            max_board_debugprint(board);
            printf("DISAGREEMENT: %X->%X known: %i, pin: %i\n", move.from, move.to, known_good, pincheck);
            exit(-1);
        }
        //}
    }*/


    /*max_piececode_t piece = board->pieces[move.from];
    if((piece & MAX_PIECECODE_KING)) {
        if(max_board_square_is_attacked(board, move.to, (MAX_PIECECODE_WHITE << board->ply) & 1)) {
            return false;
        }
    } else if(move.attr == MAX_MOVE_EN_PASSANT) {
    } else {
        return max_board_valid_line(board, piece, move.from, kpos);
    }*/
}
