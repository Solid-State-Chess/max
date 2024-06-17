#pragma once


#include "max/board/dir.h"
#include "max/board/move.h"


/// \ingroup movegen
/// @{

/// \name King Movegen
/// @{

/// Length of the #MAX_KING_MOVES array
#define MAX_KING_MOVES_LEN (8)

/// Array of offsets to apply for all 8 king moves.
/// Internally, this is a pointer to MAX_0x88_RAYS used for clarity in movegen.
/// \see MAX_KING_MOVES_LEN
extern max_0x88_dir_t *MAX_KING_MOVES;

/// Location that a king would be placed after castling on a given side for both white and black
extern max_0x88_t MAX_CASTLE_KING_DEST[MAX_CASTLES_LEN][MAX_SIDES_LEN];

/// Location that a rook would be placed after castling on the given side for white and black
extern max_0x88_t MAX_CASTLE_ROOK_DEST[MAX_CASTLES_LEN][MAX_SIDES_LEN];


/// @}


/// @}
