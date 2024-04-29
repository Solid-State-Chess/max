/// \file zobrist.h

#pragma once

#include <stdint.h>

/// \ingroup board
/// @{

/// A hash key that uniquely identifies a chess position, including factors like en passant availability and
/// castle rights.
/// These keys are used to efficiently compute draws by threefold repetition, and in the chess engine, we use this
/// hash key to index a transposition table array - see the engine transposition table section for more on this topic.
typedef uint32_t max_zobrist_t;


/// @}
