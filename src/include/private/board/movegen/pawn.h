#pragma once

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

/// @}

/// @}
