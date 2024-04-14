/// @file
/// Definition of the max engine's internal chessboard representation based on 0x88.
///
/// @see max_board_new()

#pragma once
#include "max/def.h"
#include "move.h"
#include "piece.h"
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

/// Structure storing all irreversible state that the board is capable of restoring
typedef struct {
    /// Data for if and where check is delivered from for the side to play
    max_check_t check;
    /// Castle rights and en passant state packed into one byte
    max_plyplate_t packed_state;
} max_irreversible_t;

/// A stack structure caching all state that cannot be *efficiently* recomputed with each move unmake
/// Stored up to MAX_BOARD_REVERSE_LEN
typedef struct {
    /// Array of all reversible state, indexed by the difference between current game ply and last reset
    /// ply count
    max_irreversible_t *array;
    /// The ply count that the stack was last reset at, index into stack array is index = ply - reset_ply
    uint16_t plies_since_reset;
} max_irreversible_stack_t;

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

    /// Tracking for the last captured piece for move unmaking
    max_board_capturestack_t captures;
    
    /// A stack of all irreversible state used to unmake moves efficiently without
    /// recomputing too much state
    max_irreversible_stack_t stack;
    
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

//// Get the direction (up or down) that enemy pawns will advance
MAX_INLINE_ALWAYS max_increment_t max_board_get_friendly_pawn_advance_dir(max_board_t *board) {
    return MAX_PAWN_DIR[(board->ply & 1)];
}

// Get the direction (up or down) that enemy pawns will advance
MAX_INLINE_ALWAYS max_increment_t max_board_get_enemy_pawn_advance_dir(max_board_t *board) {
    return MAX_PAWN_DIR[(board->ply & 1) ^ 1];
}

/// Generate all pseudo-valid moves for the current side to move on the given board
void max_board_movegen_pseudo(max_board_t *const board, max_movelist_t *const moves);

/// Generate all pseudo-legal capture moves for the side to play
void max_board_capturegen_pseudo(max_board_t *const board, max_movelist_t *const moves);

/// Check if the given move is legal - it does not leave the king in check
bool max_board_move_is_valid(max_board_t *const board, max_move_t move);

/// Get the current castle rights, check, and en passant state for this board
MAX_INLINE_ALWAYS max_irreversible_t* max_board_state(max_board_t const *const board) {
    return &board->stack.array[board->stack.plies_since_reset - 1];
}

/// Reset the given board's state stack pointer- after calling this moves made before the call must not be unmade
MAX_INLINE_ALWAYS void max_board_reset_stack(max_board_t *const board) {
    max_irreversible_t latest = *max_board_state(board);
    board->stack.plies_since_reset = 1;
    board->stack.array[0] = latest;
}

MAX_INLINE_ALWAYS void max_irreversible_stack_push(max_irreversible_stack_t *stack, max_irreversible_t state) {
    stack->array[stack->plies_since_reset] = state;
    stack->plies_since_reset += 1;
}

MAX_INLINE_ALWAYS max_irreversible_t max_irreversible_stack_pop(max_irreversible_stack_t *stack) {
    stack->plies_since_reset -= 1;
    return stack->array[stack->plies_since_reset];
}

/// Create a new chessboard structure initialized to the starting position,
/// using the given buffer as the irreversible state buffer - the size of this buffer
/// determines how many moves can be unmake between state resets
void max_board_new(max_board_t *const board, max_irreversible_t *ply_buf);

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

/// Check if the given squares are on the same line with no pieces between them
MAX_INLINE_ALWAYS bool max_board_is_empty_between(max_board_t *const board, max_bpos_t pos, max_bpos_t to) {
    max_increment_t dir = MAX_DIRECTION_BY_DIFF[max_bpos_diff(pos, to)];
    if(dir == 0) {
        return false;
    }

    do {
        pos = max_bpos_inc(pos, dir);
    } while(board->pieces[pos] == MAX_PIECECODE_EMPTY);

    return pos == to;
}

/// Check if the piece on the given square is attacked
uint8_t max_board_attacks(max_board_t *const board, max_bpos_t pos, max_checker_t *attacks);

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
