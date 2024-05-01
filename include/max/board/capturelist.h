/// \file capturelist.h

#pragma once

#include "max/board/piececode.h"

/// \ingroup board
/// @{

/// \ingroup state
/// @{

/// Number of pieces that will ever be on a chessboard
#define MAX_BOARD_PIECES_COUNT (32)

/// A stack of all captured pieces built during move making and unmaking.
/// This allows capture move unmaking by popping the last captured piece from the stack and 
/// placing it back into its old spot.
typedef struct {
    /// Backing array storing the piece type and color of each capture made.
    /// The capacity of this array is allowed to be two lower than the maximum number of pieces on a 
    /// chessboard because we assume that a king will never be captured in the course of a game.
    max_piececode_t pieces[MAX_BOARD_PIECES_COUNT - 2];
    /// The length of the stack (NOT the head index).
    uint8_t len;
} max_captures_t;

/// @}

/// @}
