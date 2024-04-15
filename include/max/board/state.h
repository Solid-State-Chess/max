/// \file
/// Definitions for structures representing the state of a chess game - castle rights,
/// en passant availability, single and double check, and captured pieces.
/// These structures are each stored in some collection to enable move unmaking - 
/// reverting the state of a board back to a previous state by saving state throughout the game.
///
/// \see #max_board_capturestack_t
/// \see #max_irreversible_stack_t

#pragma once
#include "max/board/piece.h"

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

/// Structure representing a piece delivering check or absence of check.
/// Includes necessary data to determine 
/// move validity - including a #max_line_t for sliding attacks including the ray that
/// must be exited in order to avoid check with a king move. For non sliding 'jumping' pieces, only
/// the board position is stored.
///
/// \section discriminant Determining Jumping or Sliding Attack
/// Because both a board position and ray are stored in the union, operations on this structure
/// assume that the origin board position is the *first* element of #max_line_t. In order to 
/// determine if an check is delivered from a sliding piece, we read the value of the increment direction
/// of the ray - if 0, then check is delivered from a jumping piece, and if valid, then it is delivered from a
/// sliding piece
///
/// \section optional Determining If Check is Detected
/// If the origin board position for either jumping or sliding attackers is invalid, then no check is delivered
///
/// \see max_checker_invalid()
/// \see max_checker_is_valid()
/// \see max_checker_is_sliding()
/// \see max_checker_is_jumping()
/// \see #max_check_t
/// \see #max_irreversible_t
typedef struct {
    union {
        /// Board position of a jumping attacker - a knight, king, or pawn
        max_bpos_t jump;
        /// Bishop, rook, or queen is delivering check on the given ray
        max_line_t ray;
    } attack;
} max_checker_t;

/// Because double check is possible, we must store two separate check structures.
/// It is guaranteed that the first check structure is filled before the second during check detection.
///
/// \see #max_irreversible_t
typedef struct {
    max_checker_t attacks[2];
} max_check_t;

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

/// Return a checker structure representing the absence of delivered check
MAX_INLINE_ALWAYS
max_checker_t max_checker_empty(void) {
    return (max_checker_t){
        .attack.ray = (max_line_t){
            .origin = MAX_BPOS_INVALID_MASK,
            .line = 0
        }
    };
}

/// Return true if the given checker structure represents check delivered by a sliding piece
MAX_INLINE_ALWAYS
bool max_checker_is_sliding(max_checker_t checker) { return checker.attack.ray.line != 0; }

/// Return true if the given checker structure represents check delivered by a jumping piece
MAX_INLINE_ALWAYS
bool max_checker_is_jumping(max_checker_t checker) { return checker.attack.ray.line == 0; }

/// Check if the given checker structure represents a valid checking piece
MAX_INLINE_ALWAYS
bool max_checker_is_valid(max_checker_t checker) { return max_bpos_valid(checker.attack.jump); }

/// Return true if either one or two pieces are delivering check to the side to play
MAX_INLINE_ALWAYS
bool max_check_exists(max_check_t check) {
    return max_checker_is_valid(check.attacks[0]);
}

/// Return true if the given checks structure contains two valid pieces delivering check
MAX_INLINE_ALWAYS
bool max_check_is_double(max_check_t check) {
    return max_checker_is_valid(check.attacks[0]) && max_checker_is_valid(check.attacks[1]);
}

/// Reset the given detected checks structure so that it represents the absence of delivered check
MAX_INLINE_ALWAYS
void max_check_reset(max_check_t *check) {
    check->attacks[0] = max_checker_empty();
    check->attacks[1] = max_checker_empty();
}
