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


/// A plate in the history stack of a `max_board_t` tracking irreversible parts of the game to allow
/// move unmaking
///
/// [2 bits for black castle rights] - [2 bits for white castle rights] - [4 bits for en passant file index]
typedef uint8_t max_plyplate_t;

enum {
    /// Mask for 4 bits representing en passantable file (invalid file means no en passant in possible)
    MAX_PLYPLATE_EP_MASK     = 0x0F,
    MAX_PLYPLATE_EP_INVALID  = MAX_PLYPLATE_EP_MASK,
    MAX_PLYPLATE_WCASTLEMASK = 0x30,
    MAX_PLYPLATE_BCASTLEMASK = 0xC0,
    MAX_PLYPLATE_QCASTLE     = 0x01,
    MAX_PLYPLATE_KCASTLE     = 0x02,
    /// Amount to bit shift `MAX_PLYPLATE_QCASTLE` to reach the white castle right bits
    MAX_PLYPLATE_CASTLE_OFFSET = 4,
};

/// Chessboard representation loosely based on 'New Architectures in Computer Chess' by Fritz Reul
typedef struct {
    /// Piece lists for white and black, tracking list position by square and square position by list position
    union {
        struct {
            max_pieces_t white;
            max_pieces_t black;
        };

        max_pieces_t sides[2];
    };
    
    /// Piece code 0x88 array for directly looking up piece by square
    max_piececode_t pieces[MAX_BOARD_0x88_LEN];
    
    /// A stack of all irreversible game state
    max_plyplate_t stack[MAX_BOARD_MAX_PLY];
    
    /// Tracking for the last captured piece for move unmaking
    max_board_capturestack_t captures;
    
    /// Detected single and double checks on the side to play, set after each move is made
    max_check_t check;
    
    /// Ply (halfmove) counter, if the LSB is set (ply is odd) then black is to move
    uint16_t ply;
} max_board_t;

/// A value of either 0 or 1 to indicate the current side to move - 0 is white and 1 is black
typedef struct { uint8_t v; } max_turn_t;


/// Get the piece list of the side that is currently to move
MAX_INLINE_ALWAYS max_pieces_t* max_board_get_to_move(max_board_t *board) {
    return &board->sides[(board->ply & 1)];
}

/// Get side state for the side that is not to move this ply
MAX_INLINE_ALWAYS max_pieces_t* max_board_get_enemy(max_board_t *board) {
    return &board->sides[(board->ply & 1) ^ 1];
}

/// Get the king's position for the side to play
MAX_INLINE_ALWAYS max_bpos_t max_board_king_pos(max_board_t *board) {
    return max_board_get_to_move(board)->king.pos[0];
}

/// Get a colormask for friendly pieces
MAX_INLINE_ALWAYS max_piececode_t max_board_friendly_colormask(max_board_t *board) {
    return MAX_PIECECODE_WHITE << (board->ply & 1);
}

/// Get the position of the friendly king
MAX_INLINE_ALWAYS max_bpos_t max_board_get_king_pos(max_board_t *board) {
    return max_board_get_to_move(board)->king.pos[0];
}

/// Get the direction (up or down) that enemy pawns will advance
MAX_INLINE_ALWAYS max_increment_t max_board_get_enemy_pawn_advance_dir(max_board_t *board) {
    return MAX_PAWN_DIR[(board->ply & 1) ^ 1];
}

/// Generate all pseudo-valid moves for the current side to move on the given board
void max_board_movegen_pseudo(max_board_t *const board, max_movelist_t *const moves);

/// Generate all pseudo-legal capture moves for the side to play
void max_board_capturegen_pseudo(max_board_t *const board, max_movelist_t *const moves);

/// Check if the given move is legal - it does not leave the king in check
bool max_board_move_is_valid(max_board_t *const board, max_move_t move);

/// Reset the given chessboard to the starting configuration
void max_board_new(max_board_t *const board);

/// Make the given move on a chessboard, with NO CHECK for move validity (assumes valid moves taken from max_movegen)
void max_board_make_move(max_board_t *const board, max_move_t move);

/// Unmake the given move, restoring any captured pieces and ep / castling state
void max_board_unmake_move(max_board_t *const board, max_move_t move);

/// Travel outwards from the given square and check for pieces of the opposite color to 
/// `attacked` that match the given piece type mask (for matching only diagonal / cardinal attackers)
/// returns true if there is an attacker on the given line
bool max_board_get_sliding_attack(
    max_board_t *board,
    max_bpos_t attacked,
    max_piececode_t typemask,
    max_increment_t direction,
    max_line_t *attack
);

/// Check if the piece on the given square is attacked
bool max_board_attacked(max_board_t *const board, max_bpos_t pos);

/// Check if the given piece is pinned to the king
bool max_board_is_pinned(max_board_t *const board, max_bpos_t from);

bool max_board_move_exits_pin(
    max_board_t *const board,
    max_piececode_t piece,
    max_bpos_t from,
    max_bpos_t to
);

/// Add a piece to the given capture stack
MAX_INLINE_ALWAYS void max_capturestack_push(max_board_capturestack_t *stack, max_piececode_t piece) {
    stack->captures[stack->head] = piece;
    stack->head += 1;
}

/// Pop the top element from the capture stack
MAX_INLINE_ALWAYS max_piececode_t max_capturestack_pop(max_board_capturestack_t *stack) {
    stack->head -= 1;
    max_piececode_t piece = stack->captures[stack->head];
    return piece;
}

#if defined(MAX_CONSOLE)

/// Print the given chessboard to the console
void max_board_debugprint(max_board_t const* board);

#endif
