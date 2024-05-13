#pragma once

#include "max/board/board.h"
#include "max/board/move.h"
#include "max/board/piececode.h"
#include <stdbool.h>


/// \ingroup movegen
/// @{

/// \name Private Functions
/// @{

/// Generate a normal attack move for a piece on the given source square towards another square.
/// This will add a quiet move if there is no piece on the given destination square, or add a capture
/// on the given square if a piece not matching the given bitmask occupies the destination square.
/// Note that pawns do NOT follow this convention, as they can only move to their attack squares if they are occupied by an enemy.
/// \param board The board to generate moves from
/// \param list The list to add the capture of quiet move to
/// \param enemy A bitmask that matches enemy pieces of the piece on #from
/// \param from The square that a piece originates from
/// \param to The square that is attacked by the piece on #from
/// \return false If the destination square was invalid or if any piece (enemies included) occupied the given square (used for sliding movegen)
bool max_board_movegen_attack(max_board_t *board, max_movelist_t *list, max_piecemask_t enemy, max_0x88_t from, max_0x88_t to);

/// Update check for the side to play.
/// This method updates the current state stack head's check structures to reflect the status of check after a a piece
/// on the given source square is moved to a given destination square.
/// It is meant to be called exclusively from the max_board_make_move() function, and does not need to be considered in
/// move unmaking because we can just reuse the check structure stored on the stack.
void max_board_update_check(max_board_t *board, max_smove_t move);


/// @}

/// @}
