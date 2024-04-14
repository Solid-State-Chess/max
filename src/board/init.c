#include "max/board.h"
#include "max/piece.h"
#include "max/square.h"
#include "private.h"
#include <stdlib.h>
#include <string.h>


static void max_board_init_lists(max_board_t *const board);

void max_board_new(max_board_t *const board, max_irreversible_t *stack) {
    board->ply = 0;
    memset(&board->pieces, MAX_PIECECODE_INVAL, sizeof(board->pieces));
    memset(board->sides, 0, sizeof(board->sides));
    memset(&board->captures, 0, sizeof(board->captures));
    
    for(unsigned i = MAX_A2; i <= MAX_H2; i += 1) {
        board->pieces[i] = MAX_PIECECODE_PAWN | MAX_PIECECODE_WHITE;
    }

    for(unsigned i = MAX_A7; i <= MAX_H7; i += 1) {
        board->pieces[i] = MAX_PIECECODE_PAWN | MAX_PIECECODE_BLACK;
    }

    #define SYMMETRY(r1, r2, kind) do {                                               \
        board->pieces[MAX_##r1##1] = board->pieces[MAX_##r2##1] = kind | MAX_PIECECODE_WHITE; \
        board->pieces[MAX_##r1##8] = board->pieces[MAX_##r2##8] = kind | MAX_PIECECODE_BLACK; \
    } while(0)

    SYMMETRY(A, H, MAX_PIECECODE_ROOK); 
    SYMMETRY(B, G, MAX_PIECECODE_KNIGHT);
    SYMMETRY(C, F, MAX_PIECECODE_BISHOP);

    #undef SYMMETRY

    board->pieces[MAX_D1] = MAX_PIECECODE_QUEEN | MAX_PIECECODE_WHITE;
    board->pieces[MAX_D8] = MAX_PIECECODE_QUEEN | MAX_PIECECODE_BLACK;

    board->pieces[MAX_E1] = MAX_PIECECODE_KING | MAX_PIECECODE_WHITE;
    board->pieces[MAX_E8] = MAX_PIECECODE_KING | MAX_PIECECODE_BLACK;

    max_board_init_lists(board);
    

    board->stack.array = stack;
    board->stack.plies_since_reset = 1;

    max_irreversible_t *state = &board->stack.array[0];
    state->packed_state = MAX_PLYPLATE_WCASTLEMASK | MAX_PLYPLATE_BCASTLEMASK | MAX_PLYPLATE_EP_INVALID;
    max_check_reset(&state->check);
}

/// Initialize piece lists for each side by iterating through each square and set all valid empty squares to the proper piece code
static void max_board_init_lists(max_board_t *const board) {
    for(uint8_t x = 0; x < 8; ++x) {
        for(uint8_t y = 0; y < 8; ++y) {
            max_bpos_t pos = max_bpos_new(x, y);
            max_piececode_t piece = board->pieces[pos];
            
            if(piece == MAX_PIECECODE_INVAL) {
                board->pieces[pos] = MAX_PIECECODE_EMPTY;
                continue;
            }
            
            max_pieces_t *side = &board->sides[(piece & MAX_PIECECODE_BLACK) >> 6];
            max_board_add_piece(board, side, pos, piece);
        }
    }
}



static MAX_INLINE_ALWAYS int8_t sign(int8_t n) {
    return (n & 0b10000000) ? -1 : 1;
}

void max_init_statics(void) {
    for(int8_t x = 0; x < 8; ++x) {
        for(int8_t y = 0; y < 8; ++y) {
            max_bpos_t from = max_bpos_new(x, y);

            for(int8_t to_x = 0; to_x < 8; ++to_x) {
                for(int8_t to_y = 0; to_y < 8; ++to_y) {
                    max_bpos_t to = max_bpos_new(to_x, to_y);

                    int8_t xd = to_x - x;
                    int8_t yd = to_y - y;

                    max_increment_t direction = 0;
                    if(x == to_x && y != to_y) {
                        direction = 0x10 * sign(yd);
                    } else if(y == to_y) {
                        direction = 0x01 * sign(xd); 
                    }

                    if(abs(xd) == abs(yd) && xd != 0) {
                        direction = (0x10 * sign(yd)) + (0x01 * sign(xd));
                    }

                    MAX_DIRECTION_BY_DIFF[max_bpos_diff(from, to)] = direction;
                }
            }
        }
    }

    MAX_DIRECTION_BY_DIFF[0x77] = 0;
}