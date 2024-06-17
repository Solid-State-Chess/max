#pragma once


#include "max/board/board.h"
#include "max/board/move.h"


/// \ingroup movegen
/// @{

/// \name King Movegen
/// @{

/// Perform castling movegen for the rook located on the given square, scanning to ensure that the
/// space between king and rook is empty and that the destination squares are empty for both pieces.
/// Follows chess960 castling rules, so rooks do not necessarily need to be on the default starting squares.
void max_board_movegen_castle(max_board_t *board, max_movelist_t *movelist, max_pieces_t *side, max_castle_side_t castle_side);


/// @}

/// @}
