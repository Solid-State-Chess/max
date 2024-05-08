/// \file movegen.h

#pragma once


#include "max/board/board.h"
#include "max/board/move.h"

/// \ingroup board
/// @{

/// \defgroup movegen Move Generation, Making, Unmaking
/// Functions to generate and validate moves on a board, as well as making/unmaking those moves
/// @{

/// Fill a movelist with all pseudolegal moves for the current side to play.
/// Generated moves are not checked for legality, and must be validated before being played
/// on the board.
void max_board_movegen(max_board_t *board, max_movelist_t *list);

/// Check if the given pseudo-legal move is valid on the board - that is, it does not leave a
/// king in check and doesn't exit a pin line
bool max_board_legal(max_board_t *board, max_smove_t move);

/// Make the given move on the board, changing the ply counter, adjusting any captures made,
/// and pushing a new state to the stack.
/// \note The passed move must be checked for legality before the move is made
void max_board_make_move(max_board_t *board, max_smove_t move);

/// Unmake the given move on the board, reverting the state back to before it was played.
/// \param move The last move that was played
void max_board_unmake_move(max_board_t *board, max_smove_t move);

/// @}

/// @}
