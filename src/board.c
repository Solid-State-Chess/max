#include "max/board.h"
#include "max/move.h"
#include "max/square.h"
#include <string.h>

/// Lookup table to convert between 0 based LERF square indices and 10x12 board indices
/// and to iterate through valid squares on a 10x12 board efficiently
const uint8_t lookup_index_10x12[64] = {
    21, 22, 23, 24, 25, 26, 27, 28,
    31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48,
    51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68,
    71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88,
    91, 92, 93, 94, 95, 96, 97, 98
};

void max_board_new(max_board_t *const board) {
    memset(board->grid, MAX_INVALID_SQUARE, sizeof(board->grid));

    //0-initializing the board makes all squares invalid
    for(uint8_t i = 0; i < 64; ++i) {
        board->grid[lookup_index_10x12[i]] = MAX_EMPTY_SQUARE;
    }

    memset(board->capture_stack.array, MAX_PIECE_EMPTY, sizeof(board->capture_stack.array));
    board->capture_stack.head = 0;
}

void max_board_reset(max_board_t *const board) {
    max_board_new(board);
    
    for(unsigned i = MAX_A2; i <= MAX_H2; i += 1) {
        board->grid[i] = MAX_WPAWN | MAX_COLOR_WHITE;
    }

    for(unsigned i = MAX_A7; i <= MAX_H7; i += 1) {
        board->grid[i] = MAX_BPAWN | MAX_COLOR_BLACK;
    }

    #define SYMMETRY(r1, r2, kind) do {                                               \
        board->grid[MAX_##r1##1] = board->grid[MAX_##r2##1] = kind | MAX_COLOR_WHITE; \
        board->grid[MAX_##r1##8] = board->grid[MAX_##r2##8] = kind | MAX_COLOR_BLACK; \
    } while(0)

    SYMMETRY(A, H, MAX_ROOK); 
    SYMMETRY(B, G, MAX_KNIGHT);
    SYMMETRY(C, F, MAX_BISHOP);


    #undef SYMMETRY

    board->grid[MAX_D1] = MAX_QUEEN | MAX_COLOR_WHITE;
    board->grid[MAX_D8] = MAX_QUEEN | MAX_COLOR_BLACK;

    board->grid[MAX_E1] = MAX_KING | MAX_COLOR_WHITE;
    board->grid[MAX_E8] = MAX_KING | MAX_COLOR_BLACK;
}

void max_board_make_move(max_board_t *const board, max_move_t move) {
    switch(move.attr) {
        case MAX_MOVE_NORMAL: {
            max_square_t captured = board->grid[move.to];
            if(captured != MAX_PIECE_EMPTY) {
                board->capture_stack.array[board->capture_stack.head] = board->grid[move.to];
                board->capture_stack.head += 1;
            }

            board->grid[move.to] = board->grid[move.from];
            board->grid[move.from] = MAX_EMPTY_SQUARE;
        } break;

        case MAX_MOVE_CAPTURE: {
                        board->grid[move.to] = board->grid[move.from];
            board->grid[move.from] = MAX_EMPTY_SQUARE;
        } break;

        case MAX_MOVE_KCASTLE: {

        } break;
    }
}

#if !defined(MAX_CONSOLE)

#include <stdio.h>

static char piece_chars[8] = {
    [MAX_PIECE_EMPTY] = '-',
    [MAX_WPAWN]  = 'p',
    [MAX_BPAWN]  = 'p',
    [MAX_KNIGHT] = 'n',
    [MAX_BISHOP] = 'b',
    [MAX_ROOK]   = 'r',
    [MAX_QUEEN]  = 'q',
    [MAX_KING]   = 'k'
};

void max_board_debugprint(max_board_t const* board) {
    for(uint8_t rank = 8; rank >= 1; --rank) {
        for(uint8_t file = 1; file <= 8; ++file) {
            max_square_t sq = board->grid[rank * 10 + 10 + file];
            char code = piece_chars[sq & MAX_PIECECODE_MASK];

            if((sq & MAX_PIECECODE_MASK) != MAX_PIECE_EMPTY && (sq & MAX_COLOR_MASK) != MAX_COLOR_WHITE) {
                code = (code - 'a') + 'A';
            }

            putc(code, stdout);
        }
        putc('\n', stdout);
    }
}

#endif
