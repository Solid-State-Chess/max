#pragma once
#include "max/def.h"
#include "max/move.h"
#include "max/piece.h"
#include <stdint.h>


enum {
    /// Bitmask for a stack plate with the piece code, if any, of a captured piece with color bit
    MAX_STATE_CAPTURED_MASK = 0b00001111,
    MAX_STATE_WCASTLE_MASK  = 0b00110000,
    MAX_STATE_BCASTLE_MASK  = 0b11000000,
};

typedef uint8_t max_state_t;

typedef uint8_t max_lidx_t;

/// Chessboard representation loosely based on 'New Architectures in Computer Chess' by Fritz Reul
typedef struct {
    max_pieces_t white;
    max_pieces_t black;
    max_lidx_t white_index[128];
    max_lidx_t black_index[128];
    max_piececode_t pieces[128];
    
    /// Stack for all irreversible state changed when making moves
    max_state_t stack[MAX_BOARD_MAX_PLY];
    
    /// Current ply count, indexes the state stack
    /// If the LSB is set (ply is odd), then black is to move
    uint16_t ply;
} max_board_t;


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
