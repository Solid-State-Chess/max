#pragma once
#include <stdint.h>
#include <stddef.h>
#include "max/def.h"
#include "max/square.h"

#define MAX_TOTAL_PAWNS (8)
#define MAX_TOTAL_KNIGHTS (10)
#define MAX_TOTAL_BISHOPS (10)
#define MAX_TOTAL_ROOKS (10)
#define MAX_TOTAL_QUEENS (9)

/// Index into a piece list
typedef uint8_t max_lidx_t;

/// A variable-sized piece list with length and array of pieces
typedef struct {
    max_lidx_t len;
    max_bpos_t pos[];
} max_piecelist_t;


#define PIECELIST(size) struct { max_lidx_t len; max_bpos_t pos[(size)]; }

/// A complete list of the positions of all the pieces on one side of a chess game
typedef struct {
    PIECELIST(MAX_TOTAL_PAWNS) pawns;
    PIECELIST(MAX_TOTAL_KNIGHTS) knights;
    PIECELIST(MAX_TOTAL_BISHOPS) bishops;
    PIECELIST(MAX_TOTAL_ROOKS) rooks;
    PIECELIST(MAX_TOTAL_QUEENS) queens;
    PIECELIST(1) king;
    /// Position to index map
    max_lidx_t index[MAX_BOARD_0x88_LEN];
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

/// Get the position array for a piece given by the piece code
MAX_INLINE_ALWAYS max_piecelist_t* max_pieces_get_list(max_pieces_t *pieces, max_piececode_t piece) {
    static const uint8_t lookup[32] = {
        [MAX_PIECECODE_PAWN]   = offsetof(max_pieces_t, pawns),
        [MAX_PIECECODE_KNIGHT] = offsetof(max_pieces_t, knights),
        [MAX_PIECECODE_BISHOP] = offsetof(max_pieces_t, bishops),
        [MAX_PIECECODE_ROOK]   = offsetof(max_pieces_t, rooks),
        [MAX_PIECECODE_QUEEN]  = offsetof(max_pieces_t, queens),
        [MAX_PIECECODE_KING]   = offsetof(max_pieces_t, king)
    };

    return (max_piecelist_t*)((void*)pieces + lookup[piece & MAX_PIECECODE_TYPE_MASK]);
}
