#include "max.h"
#include "max/board.h"
#include "max/def.h"
#include "max/move.h"
#include <stdbool.h>

MAX_INLINE_ALWAYS bool empty(max_square_t square) {
    return square != 0 && (square & MAX_PIECECODE_MASK) == 0;
}

void max_movegen(max_movelist_t *const moves, max_board_t *const board) {
    static void** JUMPTBL[8] = {
        [MAX_WPAWN]  = &&wpawn,
        [MAX_BPAWN]  = &&bpawn,
        [MAX_KNIGHT] = &&knight,
        [MAX_BISHOP] = &&bishop,
        [MAX_ROOK]   = &&rook,
        [MAX_QUEEN]  = &&queen,
        [MAX_KING]   = &&king,
    };

    for(max_square_idx_t k = 0; k < 64; ++k) {
        max_square_idx_t i = lookup_index_10x12[k];
        max_square_t piece = board->grid[i];
        uint8_t code = piece & MAX_PIECECODE_MASK;
        if(code == 0) {
            continue;
        }

        goto *JUMPTBL[code];

        #define PAWN_MOVEGEN(dir, enemy) do {                                                           \
            uint8_t next = i dir 10;                                                                    \
            max_square_t above = board->grid[next];                                                     \
            if(above != MAX_INVALID_SQUARE) {                                                           \
                if(above == MAX_EMPTY_SQUARE) {                                                         \
                    max_movelist_add(moves, (max_move_t){.from = i, .to = next, .flags = 0});           \
                }                                                                                       \
                max_square_t right = board->grid[next + 1];                                             \
                max_square_t left  = board->grid[next - 1];                                             \
                if(right > MAX_EMPTY_SQUARE && (right & MAX_COLOR_MASK) == enemy) {                     \
                    max_movelist_add(moves, (max_move_t){.from = i, .to = next + 1, .flags = 0});       \
                }                                                                                       \
                if(right > MAX_EMPTY_SQUARE && (left & MAX_COLOR_MASK) == enemy) {                      \
                    max_movelist_add(moves, (max_move_t){.from = i, .to = next - 1, .flags = 0});       \
                }                                                                                       \
            }                                                                                           \
        } while(0)

    wpawn:
        PAWN_MOVEGEN(+, MAX_COLOR_BLACK);
        continue;
    bpawn:
        PAWN_MOVEGEN(-, MAX_COLOR_WHITE);
        continue;

    knight: {
        uint8_t color = piece & MAX_COLOR_MASK;
        #define KNIGHTATTACK(idx) do {                                                                  \
            max_square_t sq = board->grid[(idx)];                                                       \
            if(sq >= MAX_EMPTY_SQUARE && (sq & MAX_COLOR_MASK) != color || sq == MAX_EMPTY_SQUARE) {    \
                max_movelist_add(moves, (max_move_t){.from = i, .to = (idx), .flags = 0});              \
            }                                                                                           \
        } while(0)

        KNIGHTATTACK(i + 21);
        KNIGHTATTACK(i + 19);
        KNIGHTATTACK(i - 19);
        KNIGHTATTACK(i - 21);

        KNIGHTATTACK(i + 12);
        KNIGHTATTACK(i - 8);
        KNIGHTATTACK(i + 8);
        KNIGHTATTACK(i - 12);
        
    }
    bishop:

    rook:

    queen:

    king:
        #undef PAWN_MOVEGEN
    
    }
}
