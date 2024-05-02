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

/// Length of the #MAX_PAWN_ATTACK_SIDES array
#define MAX_PAWN_ATTACK_SIDE_LEN (2)

/// Sides that a pawn may attack towards, must be combined with the appropriate element
/// of #MAX_PAWN_ADVANCE_DIR to form a correct pawn attack square
max_0x88_dir_t MAX_PAWN_ATTACK_SIDES[MAX_PAWN_ATTACK_SIDE_LEN];

/// Directions that a pawn from a given side will advance towards
max_0x88_dir_t MAX_PAWN_ADVANCE_DIR[MAX_SIDES_LEN];

/// Promotion rank indices for pawns of the given side.
uint8_t MAX_PAWN_PROMOTE_RANK[MAX_SIDES_LEN];

/// En passant rank numbers for each side.
/// This is the rank that a pawn must be located on in order to capture by en
/// passant if an enemy pawn has just moved adjacently.
uint8_t MAX_PAWN_EP_RANK[MAX_SIDES_LEN];

/// Generate pseudo-legal moves for all pawns of the given side
/// \param board The board to generate moves on
/// \param list Move list to add pseudo legal moves to
/// \param pieces Piece list for the side to play
/// \param enemy Bitmask that matches enemy pieces for the side to play
/// \param side Side that the pawn belongs to
void max_board_movegen_pawns(max_board_t *board, max_movelist_t *list, max_pieces_t *pieces, max_piecemask_t enemy, max_side_t side);

/// @}

/// @}
