#pragma once


#include "max/board/board.h"
#include "max/board/dir.h"
#include "max/board/move.h"
#include "max/board/piececode.h"


/// \ingroup movegen
/// @{

/// \name Sliding Movegen
/// @{

/// Generate a sliding attackers moves from the given source square along the provided ray.
/// \param board The board to generate moves on
/// \param list List to add quiet and attack sliding moves to
/// \param friendly A bitmask that will match friendly pieces of the piece on the source square
/// \param source The origin square of the sliding attacker
/// \param ray Increment to apply to each square along the line
void max_board_movegen_slide(max_board_t *board, max_movelist_t *list, max_piecemask_t friendly, max_0x88_t source, max_0x88_dir_t ray);

/// @}

/// @}
