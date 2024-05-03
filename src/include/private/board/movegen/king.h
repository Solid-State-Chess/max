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

/// Side that castling can be performed, either 0 or 1.
/// ```
/// 0 - A side castling
/// 1 - H side castling
/// ```
typedef uint8_t max_castle_side_t;

enum {
    MAX_CASTLE_ASIDE = 0,
    MAX_CASTLE_HSIDE = 1,
};

/// Length of arrays indexed by a castle side bit
#define MAX_CASTLES_LEN (2)

/// Array of offsets for all possible king moves
extern max_0x88_dir_t MAX_KING_MOVES[MAX_KING_MOVES_LEN];

/// Location that a king would be placed after castling on a given side for both white and black
extern max_0x88_t MAX_CASTLE_KING_DEST[MAX_CASTLES_LEN][MAX_SIDES_LEN];

/// Location that a rook would be placed after castling on the given side for white and black
extern max_0x88_t MAX_CASTLE_ROOK_DEST[MAX_CASTLES_LEN][MAX_SIDES_LEN];


/// Perform castling movegen for the rook located on the given square, scanning to ensure that the
/// space between king and rook is empty and that the destination squares are empty for both pieces
void max_board_movegen_castle(max_board_t *board, max_movelist_t *movelist, max_pieces_t *side, max_0x88_t rook, max_castle_side_t castle_side);

/// @}

/// @}
