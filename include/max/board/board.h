/// @file
/// Definition of the max engine's internal chessboard representation based on 0x88.
///
/// @see max_board_new()

#pragma once
#include "max/def.h"
#include "move.h"
#include "piece.h"
#include <stdint.h>


/// A stack containing all pieces that have been captured over the course of the game.
/// This structure is used for the same purpose as the #max_irreversible_t, but differs
/// in that it has a fixed size requirement unaffected by the desired move make / unmake depth because
/// only a fixed number of pieces can ever be captured in a game.
///
/// \see #max_irreversible_stack_t
typedef struct {
    /// Fixed size array of all pieces captured in this game
    max_piececode_t captures[MAX_BOARD_PIECES_COUNT];
    /// Length of the stack (NOT HEAD INDEX) - if this is zero, then the stack is empty
    uint8_t len;
} max_board_capturestack_t;


/// A plate in the history stack of a `max_board_t` tracking irreversible parts of the game.
/// Stored as part of the irreversible game state in a #max_irreversible_t.
///
/// [2 bits for black castle rights] - [2 bits for white castle rights] - [4 bits for en passant file index]
/// \see #MAX_PLYPLATE_EP_MASK
/// \see #MAX_PLYPLATE_WCASTLEMASK
/// \see #MAX_PLYPLATE_BCASTLEMASK
/// \see #max_irreversible_t
typedef uint8_t max_plyplate_t;

enum {
    /// Mask for the lower 4 bits of a ::max_plyplate_t containing a single file index as stored in
    /// a ::max_bpos_t. If the lower four bits are > 7, then the file is invalid and thus there is no
    /// en passant possible for this ply. Otherwise, this is set to the file on which the enemy has
    /// just moved a pawn two spaces forwards to indicate en passant is available
    MAX_PLYPLATE_EP_MASK     = 0x0F,
    /// Helper definition for an invalid en passant file as stored in a ::max_plyplate_t - marks that en
    /// passant is not possible for the side to move on this ply
    MAX_PLYPLATE_EP_INVALID  = MAX_PLYPLATE_EP_MASK,
    /// Mask for bits [4, 5] of a ::max_plyplate_t holding king and queenside castle rights for
    /// white.
    /// \see MAX_PLYPLATE_QCASTLE
    /// \see MAX_PLYPLATE_KCASTLE
    MAX_PLYPLATE_WCASTLEMASK = 0x30,
    /// Mask for bits [6, 7] of a ::max_plyplate_t holding king and queenside castle rights for black.
    MAX_PLYPLATE_BCASTLEMASK = 0xC0,
    /// Bitmask for black and white queenside castle rights - the lower bit of 
    /// #MAX_PLYPLATE_WCASTLEMASK or #MAX_PLYPLATE_BCASTLEMASK stored queenside castle
    /// availability
    MAX_PLYPLATE_QCASTLE     = 0x01,
    /// Bitmask for black and white kingside castle rights - the upper bit of a #MAX_PLYPLATE_WCASTLEMASK or
    /// #MAX_PLYPLATE_BCASTLEMASK stores kingside castle rights
    MAX_PLYPLATE_KCASTLE     = 0x02,
    /// Bit offset of #MAX_PLYPLATE_WCASTLEMASK - if #MAX_PLYPLATE_QCASTLE or #MAX_PLYPLATE_KCASTLE are shifted
    /// by this amount, then you will have a mask for WHITE king or queenside castle rights respectively
    MAX_PLYPLATE_WCASTLE_OFFSET = 4,
};

/// Structure containing cached state for the current game ply. When placed into a 
/// #max_irreversible_stack_t, this enables the board to make and unmake game moves without
/// requiring recomputation of check, castle rights, and en passant status.
/// \see #max_irreversible_stack_t
typedef struct {
    /// Data for if and where check is delivered from for the side to play
    max_check_t check;
    /// Castle rights and en passant state packed into one byte
    max_plyplate_t packed_state;
} max_irreversible_t;

/// A stack structure caching game state that cannot be *efficiently* recomputed when a move is unmade.
/// Includes castle rights, en passant file, and check status of the side to move. There are some important
/// notes around the size of this stack.
///
/// \section size Stack Size Requirements
/// The stack size determines how many sequential moves can be unmade in a row. When a move is made,
/// the old game state is pushed to this stack, to be restored by a pop when that same move is unmade.
/// In order to reduce memory footprint, this stack is bounded in size and must be *reset* periodically,
/// clearing all saved game state.
///
/// \see max_irreversible_stack_push()
/// \see max_irreversible_stack_pop()
typedef struct {
    /// Array of all reversible state, indexed by #plies_since_reset
    max_irreversible_t *array;
    /// Index of the head of the stack, incremented each time a move is made and
    /// decremented each time a move is unmade
    uint16_t plies_since_reset;
} max_irreversible_stack_t;

/// Chessboard representation loosely based on 'New Architectures in Computer Chess' [1] by Fritz Reul.
///
/// [1] https://pure.uvt.nl/ws/files/1098572/Proefschrift_Fritz_Reul_170609.pdf
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
    stack->captures[stack->len] = piece;
    stack->len += 1;
}

/// Pop the top element from the capture stack
MAX_INLINE_ALWAYS max_piececode_t max_capturestack_pop(max_board_capturestack_t *stack) {
    stack->len -= 1;
    max_piececode_t piece = stack->captures[stack->len];
    return piece;
}

#if defined(MAX_CONSOLE)

/// Print the given chessboard to the console
void max_board_debugprint(max_board_t const* board);

#endif
