#pragma once
#include "max/def.h"
#include "max/move.h"
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
    
    MAX_INVALID_SQUARE = (int8_t)0b10000000,
    MAX_EMPTY_SQUARE   = 0b00000000,
};

/// A single square stored in the 10x12 board array
typedef int8_t max_square_t;

enum {
    /// Bitmask for a stack plate with the piece code, if any, of a captured piece with color bit
    MAX_STATE_CAPTURED_MASK = 0b00001111,
    MAX_STATE_WCASTLE_MASK  = 0b00110000,
    MAX_STATE_BCASTLE_MASK  = 0b11000000,
};

typedef uint8_t max_state_t;

/// Chessboard representation with side to move, castling rights
typedef struct {
    /// Grid including sentinel ranks and files used to speed up out of bounds detection
    max_square_t grid[120];
    
    /// Stack for all irreversible state changed when making moves
    max_state_t stack[MAX_BOARD_MAX_PLY];
    
    /// Current ply count, indexes the state stack
    /// If the LSB is set (ply is odd), then black is to move
    uint16_t ply;
} max_board_t;

/// Lookup table for valid squares on a 10x12 board
extern const max_square_idx_t lookup_index_10x12[64];

/// Create an empty chessboard with NO pieces, only sentinel values in array and empty squares
/// at their valid indices
void max_board_new(max_board_t *const board);

/// Reset the given chessboard to the starting configuration
void max_board_reset(max_board_t *const board);

/// Make the given move on a chessboard, with NO CHECK for move validity (assumes valid moves taken from max_movegen)
void max_board_make_move(max_board_t *const board, max_move_t move);

/// Unmake the given move, restoring any captured pieces and ep / castling state
void max_board_unmake_move(max_board_t *const board, max_move_t move);

#if defined(MAX_CONSOLE)

/// Print the given chessboard to the console
void max_board_debugprint(max_board_t const* board);

#endif
