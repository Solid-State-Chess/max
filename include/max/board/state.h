/// \file state.h

#pragma once

#include "max/board/zobrist.h"

/// \ingroup board
/// @{


/// \defgroup Game State
/// Additional irreversible aspects of a chess game that must be maintained in order to make and unmake moves.
/// We must store the castle rights of both sides, the en passant file if any is available,
/// the current state of single or double check for the side to play, and a hash of the board's position
/// to be used when determining draws by threefold repetition.
/// @{

/// A struture representing data that must be tracked separately from the actual chess board.
/// In the #max_chessboard_t, this structure is not used directly, rather it is stored in a 
/// #max_state_stack_t.
typedef struct {
    max_zobrist_t position;
} max_state_t;

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

/// @}

/// @}
