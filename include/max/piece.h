#pragma once
#include <stdint.h>
#include "max/square.h"

#define MAX_TOTAL_PAWNS (8)
#define MAX_TOTAL_KNIGHTS (10)
#define MAX_TOTAL_BISHOPS (10)
#define MAX_TOTAL_ROOKS (10)
#define MAX_TOTAL_QUEENS (9)

/// A complete list of the positions of all the pieces on one side of a chess game
typedef struct {
    max_bidx_t pawns[MAX_TOTAL_PAWNS];
    max_bidx_t knights[MAX_TOTAL_KNIGHTS];
    max_bidx_t bishops[MAX_TOTAL_BISHOPS];
    max_bidx_t rooks[MAX_TOTAL_ROOKS];
    max_bidx_t queens[MAX_TOTAL_QUEENS];
    max_bidx_t king;

    uint8_t pawn_count;
    uint8_t knight_count;
    uint8_t bishop_count;
    uint8_t rook_count;
    uint8_t queen_count;
} max_pieces_t;

/// A piece code with color, validity, and type specifiers
typedef uint8_t max_piececode_t;

enum {
    MAX_PIECECODE_WHITE = 0b00100000,
    MAX_PIECECODE_BLACK = 0b01000000,
    MAX_PIECECODE_INVAL = 0b10000000,
    MAX_PIECECODE_EMPTY = 0b00000000,

    MAX_PIECECODE_PAWN   = 0b00000001,
    MAX_PIECECODE_KNIGHT = 0b00000010,
    MAX_PIECECODE_BISHOP = 0b00000100,
    MAX_PIECECODE_ROOK   = 0b00001000,
    MAX_PIECECODE_QUEEN  = MAX_PIECECODE_BISHOP | MAX_PIECECODE_ROOK,
    MAX_PIECECODE_KING   = 0b00010000,
    MAX_PIECECODE_TYPE_MASK   = 0b00011111,
    MAX_PIECECODE_COLOR_MASK  = 0b01100000,
};
