#pragma once
#include <stdint.h>

/// Piece codes used in the lower 3 bits of a byte position
enum {
    MAX_PIECE_EMPTY = 0,
    /// White and black pawns have separate piece codes to speed up lookup table
    /// driven move generation (pawns are the only piece whose move direction is affected by color)
    MAX_WPAWN   = 1,
    MAX_BPAWN   = 2,
    MAX_KNIGHT  = 3,
    MAX_BISHOP  = 4,
    MAX_ROOK    = 5,
    MAX_QUEEN   = 6,
    MAX_KING    = 7,
};

/// Square index packed into one byte
typedef struct {
    uint8_t file : 3;
    uint8_t rank : 3;
} max_packed_idx_t;

enum {
    MAX_PIECECODE_MASK = 0b00000111,
   
    MAX_COLOR_MASK     = 0b00001000,
    MAX_COLOR_WHITE    = 0b00001000,
    MAX_COLOR_BLACK    = 0b00000000,
    MAX_COLOR_EMPTY    = MAX_COLOR_WHITE,

    MAX_QCASTLE_MASK   = 0b000100000,
    MAX_KCASTLE_MASK   = 0b001000000,
    
    /// Sentinel value for invalid squares is zero to take advantage of
    /// ARM cortex M4 branch if zero instruction to avoid an extra comparison
    MAX_INVALID_SQUARE = 0b00000000,
    MAX_EMPTY_SQUARE   = (MAX_COLOR_EMPTY | MAX_PIECE_EMPTY)
};

typedef uint8_t max_square_t;

/// Chessboard representation with side to move, castling rights
typedef struct {
    /// Grid including sentinel ranks and files used to speed up out of bounds detection
    max_square_t grid[120];
} max_board_t;

/// Create an empty chessboard with NO pieces, only sentinel values in array and empty squares
/// at their valid indices
void max_board_new(max_board_t *const board);

/// Reset the given chessboard to the starting configuration
void max_board_reset(max_board_t *const board);

#if defined(MAX_CONSOLE)

/// Print the given chessboard to the console
void max_board_debugprint(max_board_t const* board);

#endif
