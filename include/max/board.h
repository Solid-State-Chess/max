#pragma once
#include "max/def.h"
#include "max/move.h"
#include "max/piece.h"
#include <stdint.h>


/// Stack structure holding all pieces that have been captured, used for move unmaking
typedef struct {
    max_piececode_t captures[MAX_BOARD_PIECES_COUNT];
    uint8_t head;
} max_board_capturestack_t;

/// All state for one side of a chess game
typedef struct {
    max_pieces_t piecelist;
    max_lidx_t   index[MAX_BOARD_0x88_LEN];
} max_sidestate_t;

/// A plate in the history stack of a `max_board_t` tracking irreversible parts of the game to allow
/// move unmaking
///
/// [2 bits for black castle rights] - [2 bits for white castle rights] - [4 bits for en passant file index]
typedef uint8_t max_plyplate_t;

enum {
    /// Mask for 4 bits representing en passantable file (invalid file means no en passant in possible)
    MAX_PLYPLATE_EP_MASK     = 0b00001111,
    MAX_PLYPLATE_EP_INVALID  = MAX_PLYPLATE_EP_MASK,
    MAX_PLYPLATE_WCASTLEMASK = 0b00110000,
    MAX_PLYPLATE_BCASTLEMASK = 0b11000000,
    MAX_PLYPLATE_QCASTLE     = 0b00000001,
    MAX_PLYPLATE_KCASTLE     = 0b00000010,
    /// Amount to bit shift `MAX_PLYPLATE_QCASTLE` to reach the white castle right bits
    MAX_PLYPLATE_CASTLE_OFFSET = 5,
};

/// Chessboard representation loosely based on 'New Architectures in Computer Chess' by Fritz Reul
typedef struct {
    /// Piece lists for white and black, tracking list position by square and square position by list position
    union {
        struct {
            max_sidestate_t white;
            max_sidestate_t black;
        };

        max_sidestate_t sides[2];
    };
    
    /// Piece code 0x88 array for directly looking up piece by square
    max_piececode_t pieces[MAX_BOARD_0x88_LEN];
    
    /// Tracking for the last captured piece for move unmaking
    max_board_capturestack_t captures;
    /// A stack of all irreversible game state
    max_plyplate_t stack[MAX_BOARD_MAX_PLY];
    
    /// If the LSB is set (ply is odd), then black is to move
    uint16_t ply;
} max_board_t;

/// Get the piece list of the side that is currently to move
MAX_INLINE_ALWAYS max_sidestate_t* max_board_get_to_move(max_board_t *board) {
    return &board->sides[(board->ply & 1)];
}

/// Get side state for the side that is not to move this ply
MAX_INLINE_ALWAYS max_sidestate_t* max_board_get_enemy(max_board_t *board) {
    return &board->sides[(board->ply & 1) ^ 1];
}

/// Create an empty chessboard with NO pieces, only sentinel values in array and empty squares
/// at their valid indices
void max_board_new(max_board_t *const board);

/// Reset the given chessboard to the starting configuration
void max_board_reset(max_board_t *const board);

/// Make the given move on a chessboard, with NO CHECK for move validity (assumes valid moves taken from max_movegen)
void max_board_make_move(max_board_t *const board, max_move_t move);

/// Unmake the given move, restoring any captured pieces and ep / castling state
void max_board_unmake_move(max_board_t *const board, max_move_t move);

MAX_INLINE_ALWAYS void max_capturestack_push(max_board_capturestack_t *stack, max_piececode_t piece) {
    stack->captures[stack->head] = piece;
    stack->head += 1;
}

MAX_INLINE_ALWAYS max_piececode_t max_capturestack_pop(max_board_capturestack_t *stack) {
    stack->head -= 1;
    max_piececode_t piece = stack->captures[stack->head];
    return piece;
}

#if defined(MAX_CONSOLE)

/// Print the given chessboard to the console
void max_board_debugprint(max_board_t const* board);

#endif
