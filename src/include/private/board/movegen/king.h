#pragma once


#include "max/board/board.h"
#include "max/board/dir.h"
#include "max/board/move.h"
#include "max/board/side.h"


/// \ingroup movegen
/// @{

/// \name King Movegen
/// @{

/// Length of the #MAX_KING_MOVES array
#define MAX_KING_MOVES_LEN (8)


/// Array of offsets for all possible king moves
extern max_0x88_dir_t MAX_KING_MOVES[MAX_KING_MOVES_LEN];

/// Location that a king would be placed after castling on a given side for both white and black
extern max_0x88_t MAX_CASTLE_KING_DEST[MAX_CASTLES_LEN][MAX_SIDES_LEN];

/// Location that a rook would be placed after castling on the given side for white and black
extern max_0x88_t MAX_CASTLE_ROOK_DEST[MAX_CASTLES_LEN][MAX_SIDES_LEN];


/// Perform castling movegen for the rook located on the given square, scanning to ensure that the
/// space between king and rook is empty and that the destination squares are empty for both pieces.
/// Follows chess960 castling rules, so rooks do not necessarily need to be on the default starting squares.
void max_board_movegen_castle(max_board_t *board, max_movelist_t *movelist, max_pieces_t *side, max_castle_side_t castle_side);


/// @}

/// @}
