#pragma once


#include "max/board/dir.h"
#include "max/board/side.h"


/// \ingroup movegen
/// @{

/// \name King Movegen
/// @{

/// Length of the #MAX_KING_MOVES array
#define MAX_KING_MOVES_LEN (8)

/// Array of offsets for all possible king moves
extern max_0x88_dir_t MAX_KING_MOVES[MAX_KING_MOVES_LEN];

/// Location that a king would be placed after castling A side for both white and black
extern max_0x88_t MAX_ACASTLE_KING_DEST[MAX_SIDES_LEN];

/// Location that a king would be placed after castling H side for both white and black
extern max_0x88_t MAX_HCASTLE_KING_DEST[MAX_SIDES_LEN];

/// Location that a rook would be placed after castling A side for white and black
extern max_0x88_t MAX_ACASTLE_ROOK_DEST[MAX_SIDES_LEN];

/// Location that a rook would be placed after castline H side for white and black
extern max_0x88_t MAX_HCASTLE_ROOK_DEST[MAX_SIDES_LEN];

/// @}

/// @}
