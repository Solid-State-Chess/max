/// \file pawn.h
#pragma once


#include "max/board/dir.h"
#include "max/board/side.h"

/// \ingroup board
/// @{

/// \ingroup movegen
/// @{


/// \name Pawn Lookup Tables
/// @{

/// Length of the #MAX_PAWN_ATTACK_SIDES array
#define MAX_PAWN_ATTACK_SIDE_LEN (2)

/// Sides that a pawn may attack towards, must be combined with the appropriate element
/// of #MAX_PAWN_ADVANCE_DIR to form a correct pawn attack square
extern max_0x88_dir_t MAX_PAWN_ATTACK_SIDES[MAX_PAWN_ATTACK_SIDE_LEN];

/// Directions that a pawn from a given side will advance towards
extern max_0x88_dir_t MAX_PAWN_ADVANCE_DIR[MAX_SIDES_LEN];

/// Promotion rank indices for pawns of the given side.
extern uint8_t MAX_PAWN_PROMOTE_RANK[MAX_SIDES_LEN];

/// En passant rank numbers for each side.
/// This is the rank that a pawn must be located on in order to capture by en
/// passant if an enemy pawn has just moved adjacently.
extern uint8_t MAX_PAWN_EP_RANK[MAX_SIDES_LEN];

/// Home rank for double pawn moves for each side.
extern uint8_t MAX_PAWN_HOMERANK[MAX_SIDES_LEN];

/// @}

/// @}
/// @}
