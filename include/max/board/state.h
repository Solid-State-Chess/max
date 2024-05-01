/// \file state.h

#pragma once

#include "max/board/zobrist.h"

/// \ingroup board
/// @{


/// \defgroup Game State
/// Additional irreversible aspects of a chess game that must be maintained in order to make and unmake moves
/// @{

/// The complete irreversible state of a chess game
typedef struct {
    max_zobrist_t position;
} max_state_t;

/// @}

/// @}
