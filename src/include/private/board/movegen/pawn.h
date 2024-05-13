#pragma once

#include "max/board/board.h"
#include "max/board/move.h"
#include "max/board/piececode.h"
#include "max/board/piecelist.h"
#include "max/board/side.h"
#include "max/board/dir.h"

/// \ingroup movegen
/// @{

/// \name Pawn Movegen
/// @{



/// Generate pseudo-legal moves for all pawns of the given side
/// \param board The board to generate moves on
/// \param list Move list to add pseudo legal moves to
/// \param pieces Piece list for the side to play
/// \param enemy Bitmask that matches enemy pieces for the side to play
/// \param side Side that the pawn belongs to
void max_board_movegen_pawns(max_board_t *board, max_movelist_t *list, max_pieces_t *pieces, max_piecemask_t enemy, max_side_t side);

/// @}

/// @}
