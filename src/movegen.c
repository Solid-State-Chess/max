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
    
    uint8_t color_to_move = (~board->ply & 1) << 3;
    for(max_square_idx_t k = 0; k < 64; ++k) {
        max_square_idx_t i = lookup_index_10x12[k];
        max_square_t piece = board->grid[i];
        uint8_t code = piece & MAX_PIECECODE_MASK;
        uint8_t color = piece & MAX_COLOR_MASK;
        if(code == MAX_EMPTY_SQUARE || color != color_to_move) {
            continue;
        }
        
        goto *JUMPTBL[code];

        #define PAWN_MOVEGEN(dir, enemy, homerow) do {                                                  \
            uint8_t next = i dir 10;                                                                    \
            max_square_t above = board->grid[next];                                                     \
            if(above != MAX_INVALID_SQUARE) {                                                           \
                if(above == MAX_EMPTY_SQUARE) {                                                         \
                    max_movelist_add(moves, (max_move_t){.from = i, .to = next, .attr = MAX_MOVE_NORMAL});            \
                    if((k & ~(0b00000111)) == (homerow)) {                                              \
                        uint8_t dmove = next dir 10;                                                    \
                        if(board->grid[dmove] == MAX_EMPTY_SQUARE) {                                    \
                            max_movelist_add(moves, (max_move_t){.from = i, .to = dmove, .attr = MAX_MOVE_NORMAL});   \
                        }                                                                               \
                    }                                                                                   \
                }                                                                                       \
                max_square_t right = board->grid[next + 1];                                             \
                max_square_t left  = board->grid[next - 1];                                             \
                if(right > MAX_EMPTY_SQUARE && (right & MAX_COLOR_MASK) == enemy) {                     \
                    max_movelist_add(moves, (max_move_t){.from = i, .to = next + 1, .attr = MAX_MOVE_CAPTURE});        \
                }                                                                                       \
                if(right > MAX_EMPTY_SQUARE && (left & MAX_COLOR_MASK) == enemy) {                      \
                    max_movelist_add(moves, (max_move_t){.from = i, .to = next - 1, .attr = MAX_MOVE_CAPTURE});        \
                }                                                                                       \
            }                                                                                           \
        } while(0)

    wpawn:
        PAWN_MOVEGEN(+, MAX_COLOR_BLACK, 0b00001000);
        continue;
    bpawn:
        PAWN_MOVEGEN(-, MAX_COLOR_WHITE, 0b11000000);
        continue;

    #undef PAWN_MOVEGEN

    knight: {
        #define KNIGHTATTACK(idx) do {                                                                  \
            max_square_t sq = board->grid[(idx)];                                                       \
            if(sq >= MAX_EMPTY_SQUARE && (sq & MAX_COLOR_MASK) != color || sq == MAX_EMPTY_SQUARE) {    \
                max_movelist_add(moves, (max_move_t){.from = i, .to = (idx), .attr = (sq | (sq >> 1) | (sq >> 2)) & 1}); \
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
    bishop: {
        #define RAYATTACK(dir, delta)                                                                       \
        do {                                                                                                \
            for(max_square_idx_t next = i dir delta;;i dir##= delta) {                                      \
                max_square_t sq = board->grid[next];                                                        \
                if(sq <= MAX_INVALID_SQUARE) { break; }                                                     \
                if(sq == MAX_EMPTY_SQUARE) {                                                                \
                    max_movelist_add(moves, (max_move_t){.from = i, .to = next, .attr = MAX_MOVE_NORMAL});          \
                } else {                                                                                    \
                    if((sq & MAX_COLOR_MASK) != color) {                                                    \
                        max_movelist_add(moves, (max_move_t){.from = i, .to = next, .attr = MAX_MOVE_CAPTURE});     \
                    }                                                                                       \
                    break;                                                                                  \
                }                                                                                           \
            }                                                                                               \
        } while(0)

        RAYATTACK(+, 11);
        RAYATTACK(+, 9);
        RAYATTACK(-, 9);
        RAYATTACK(-, 11);
        continue;
    }
    rook: {
        RAYATTACK(+, 10);
        RAYATTACK(-, 10);
        RAYATTACK(+, 1);
        RAYATTACK(-, 1);
        continue;
    }
    queen: {
        RAYATTACK(+, 11);
        RAYATTACK(+, 9);
        RAYATTACK(-, 9);
        RAYATTACK(-, 11);
        RAYATTACK(+, 10);
        RAYATTACK(-, 10);
        RAYATTACK(+, 1);
        RAYATTACK(-, 1);
        continue;
    }
    king: {
        KNIGHTATTACK(i + 9);
        KNIGHTATTACK(i + 10);
        KNIGHTATTACK(i + 11);
        KNIGHTATTACK(i + 1);
        KNIGHTATTACK(i - 1);
        KNIGHTATTACK(i - 11);
        KNIGHTATTACK(i - 9);

        uint8_t castle_rights = (board->stack[board->ply] >> (4 + ((board->ply & 1) << 1)));

        if((castle_rights & 1) && (board->grid[i + 1] | board->grid[i + 2]) == MAX_EMPTY_SQUARE) {
            max_movelist_add(
                moves,
                (max_move_t) {
                    .from = i,
                    .to = i + 2,
                    .attr = MAX_MOVE_KCASTLE,
                }
            );
        }
        if((castle_rights & 2) && (board->grid[i - 1] | board->grid[i - 2] | board->grid[i - 3]) == MAX_EMPTY_SQUARE) {
            max_movelist_add(
                moves,
                (max_move_t) {
                    .from = i,
                    .to = i - 3,
                    .attr = MAX_MOVE_QCASTLE
                }
            );
        }

        #undef KNIGHTATTACK
        continue;
    }
    }
}
