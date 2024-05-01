/// \file state.h

#pragma once

#include "max/board/dir.h"
#include "max/board/loc.h"
#include "max/board/side.h"
#include "max/board/zobrist.h"
#include "max/def.h"

/// \ingroup board
/// @{


/// \defgroup state Game State
/// Additional irreversible aspects of a chess game that must be maintained in order to make and unmake moves.
/// We must store the castle rights of both sides, the en passant file if any is available,
/// the current state of single or double check for the side to play, and a hash of the board's position
/// to be used when determining draws by threefold repetition.
/// @{

/// Structure representing an incrementally detected check on the king.
/// Values of the origin field determine if check is present, and values of the ray
/// field determine if check is delivered from a sliding piece along a line (used for x-ray attacks).
typedef struct {
    /// Board location where check originates from.
    /// If this square is off the board, then the check structure is considered 'empty'
    max_0x88_t origin;
    /// Direction from the origin to the king's location that a sliding attacker on 'origin'
    /// attacks the king through.
    /// If this ray is invalid (0), then the check structure represents check delivered from a jumping piece.
    max_0x88_dir_t ray;
} max_check_t;

/// Return true if the given check structure shows the absence of detected check
MAX_INLINE_ALWAYS bool max_check_is_empty(max_check_t check) {
    return check.origin.v == MAX_0x88_INVALID_MASK;
}

/// Return true if the given check structure represents valid check delivered by 
/// either a sliding piece or a jumping piece.
MAX_INLINE_ALWAYS bool max_check_has_value(max_check_t check) {
    return !max_check_is_empty(check);
}

/// Return true if check is delivered from a sliding attacker.
MAX_INLINE_ALWAYS bool max_check_is_sliding(max_check_t check) {
    return check.ray.v == MAX_0x88_DIR_INVALID;
}

/// \name Packed State
/// Castle rights and en passant availability are packed into one byte for memory saving
/// @{

/// Castling and en passant state packed into a single byte.
/// One of these is stored in every #max_state_t structure.
/// ## Structure
/// ```
/// [Black Castle Kingside][Black Castle Queenside][White Castle Kingside][White Castle Queenside][4 bits - En Passant File]
/// ```
typedef uint8_t max_packed_state_t;

enum {
    /// Specific value of the en passant file bits representing en passant is not available
    MAX_PSTATE_EPFILE_INVALID = 0x0F,
    /// Kingside castle bit, must be shifted in order to represent white or black's castle rights
    MAX_PSTATE_KINGSIDE_CASTLE = 0x01,
    /// Queenside castle bitmask, must be shifted in order to represent white or black's castle rights
    MAX_PSTATE_QUEENSIDE_CASTLE = 0x02,
    /// Bit position of white's two castle right bits
    MAX_PSTATE_WCASTLE_POS = 4,
    /// Bit position of black's two castle right bits
    MAX_PSTATE_BCASTLE_POS = 6,
};

/// Check if en passsant is possible for the given state byte
MAX_INLINE_ALWAYS bool max_packed_state_has_ep(max_packed_state_t state) {
    return (state & MAX_PSTATE_EPFILE_INVALID) == 0;
}

/// Get a bitmask to check for kingside castle rights on the given side
MAX_INLINE_ALWAYS max_packed_state_t max_packed_state_kcastle(max_side_t side) {
    uint8_t shift = MAX_PSTATE_WCASTLE_POS + (side << 1);
    return MAX_PSTATE_KINGSIDE_CASTLE << shift;
}

/// Get a bitmask to check for queenside castle rights for the given side.
MAX_INLINE_ALWAYS max_packed_state_t max_packed_state_qcastle(max_side_t side) {
    uint8_t shift = MAX_PSTATE_WCASTLE_POS + (side << 1);
    return MAX_PSTATE_QUEENSIDE_CASTLE << shift;
}

/// @}

#pragma pack(push, 1)

/// A struture representing data that must be tracked separately from the actual chess board.
/// In the #max_chessboard_t, this structure is not used directly, rather it is stored in a 
/// #max_state_stack_t.
typedef struct {
    /// Zobrist hash of this position, saved in order to determine draw by threefold repetition.
    /// Fingers crossed that someone isn't extremely unlucky and three separate key collisions occur :)
    max_zobrist_t position;
    /// An array of two separate check structures, if one or both of these structures
    /// is valid then the side to play is in check and must escape in order to continue the game.
    /// The first (index 0) check structure will always be set to indicate single check, while the second
    /// will be filled if the king is in double check.
    max_check_t check[2];
    /// Packed data for black and white castle rights and availability of en passant.
    max_packed_state_t state; 
} max_state_t;

#pragma pack(pop)

/// Get the number of detected checks against the side to play a move in the given state structure.
MAX_INLINE_ALWAYS uint8_t max_state_checks(max_state_t *state) {
    return max_check_has_value(state->check[0]) + max_check_has_value(state->check[1]);
}

/// A stack of the chess game's state, indexed by game ply.
/// This is the only structure for the #max_chessboard_t that has an indeterminate size
/// because a plate must be pushed in order for moves to be unmade - so the size of the stack
/// depends on the depth at which an engine would like to search.
///
/// Appropriately, it stores a pointer to a user-provided buffer rather than a fixed-size array
/// of state plates.
/// The capacity of this buffer is not checked during board methods - the user that is making and unmaking moves
/// must ensure that they do not exceed the depth capacity themselves.
///
/// \note
/// Because the chess board stores state in this stack, it is guaranteed that there will always be at least
/// one element in the stack to represent the state of the current ply.
typedef struct {
    /// Pointer to a buffer of game states, indexed by the #head index.
    max_state_t *plates;
    /// Index of the last valid element in the #plates buffer - this index must always be valid as
    /// the plates array is guaranteed to never be empty.
    uint8_t head;
} max_state_stack_t;

/// Get a pointer to the head of the stack - this is the current state of the game
MAX_INLINE_ALWAYS max_state_t* max_state_stack_peek(max_state_stack_t *stack) {
    return &stack->plates[stack->head];
}

/// @}

/// @}
