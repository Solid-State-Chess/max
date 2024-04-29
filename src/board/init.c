#include "max/board/board.h"
#include "max/board/piece.h"
#include "max/board/square.h"
#include "max/board/state.h"
#include "max/board/zobrist.h"
#include "private.h"
#include <stdlib.h>
#include <string.h>


static void max_board_init_zobrist(max_board_t *const board);

void max_board_new(max_board_t *const board, max_irreversible_t *stack) {
    board->ply = 0;
    memset(&board->pieces, MAX_PIECECODE_INVAL, sizeof(board->pieces));
    for(unsigned i = 0; i < 64; ++i) {
        board->pieces[max_bpos_decompress(i)] = MAX_PIECECODE_EMPTY;
    }
    board->captures.len = 0;
    memset(board->sides, 0, sizeof(board->sides));

    max_board_init_zobrist(board);
    max_irreversible_stack_new(&board->stack, stack, max_irreversible_default());
    max_board_startpos(board);
}

void max_board_clear(max_board_t *const board) {
    for(uint8_t i = 0; i < 64; ++i) {
        max_bpos_t pos = max_bpos_decompress(i);
        if(board->pieces[pos] != MAX_PIECECODE_EMPTY) {
            max_board_remove_piece(
                board,
                &board->sides[max_board_piece_side(board->pieces[pos])],
                pos
            );
        }
    }

    max_irreversible_stack_reset(&board->stack);
    *max_irreversible_stack_peek(&board->stack) = max_irreversible_default();
    board->captures.len = 0;
    board->ply = 0;
    board->zobrist.hash = 0;
}

void max_board_startpos(max_board_t *const board) {
    max_board_clear(board);

    for(unsigned i = MAX_A2; i <= MAX_H2; i += 1) {
        max_board_add_piece(board, &board->white, i, MAX_PIECECODE_PAWN | MAX_PIECECODE_WHITE);
    }

    for(unsigned i = MAX_A7; i <= MAX_H7; i += 1) {
        max_board_add_piece(board, &board->black, i, MAX_PIECECODE_PAWN | MAX_PIECECODE_BLACK);
    }

    #define SYMMETRY(r1, r2, kind) do {                                                     \
        max_board_add_piece(board, &board->white, MAX_##r1##1, kind | MAX_PIECECODE_WHITE); \
        max_board_add_piece(board, &board->white, MAX_##r2##1, kind | MAX_PIECECODE_WHITE); \
                                                                                            \
        max_board_add_piece(board, &board->black, MAX_##r1##8, kind | MAX_PIECECODE_BLACK); \
        max_board_add_piece(board, &board->black, MAX_##r2##8, kind | MAX_PIECECODE_BLACK); \
    } while(0)

    SYMMETRY(A, H, MAX_PIECECODE_ROOK); 
    SYMMETRY(B, G, MAX_PIECECODE_KNIGHT);
    SYMMETRY(C, F, MAX_PIECECODE_BISHOP);

    #undef SYMMETRY
    

    max_board_add_piece(board, &board->white, MAX_D1, MAX_PIECECODE_QUEEN | MAX_PIECECODE_WHITE);
    max_board_add_piece(board, &board->black, MAX_D8, MAX_PIECECODE_QUEEN | MAX_PIECECODE_BLACK);
    
    max_board_add_piece(board, &board->white, MAX_E1, MAX_PIECECODE_KING | MAX_PIECECODE_WHITE);
    max_board_add_piece(board, &board->black, MAX_E8, MAX_PIECECODE_KING | MAX_PIECECODE_BLACK);

}

/// Initialize zobrist hash elements with randomly generated numbers for each piece on each square
static void max_board_init_zobrist(max_board_t *const board) {
    board->zobrist.b = board->zobrist.c = board->zobrist.d = 0x69420B34;
    for (uint8_t i = 0; i < 20; ++i) {
        max_zobrist_rand((max_board_t*)&board);
    }

    for(unsigned i = 0; i < 12; ++i) {
        for(unsigned j = 0; j < 64; ++j) {
            MAX_ZOBRIST_PIECEPOS[i][j] = max_zobrist_rand(board);
        }
    }

    for(unsigned i = 0; i < 16; ++i) {
        MAX_ZOBRIST_CASTLERIGHTS[i] = max_zobrist_rand(board);
    }

    for(unsigned i = 0; i < 8; ++i) {
        MAX_ZOBRIST_EPFILE[i] = max_zobrist_rand(board);
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
