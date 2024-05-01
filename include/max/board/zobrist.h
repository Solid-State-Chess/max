/// \file zobrist.h

#pragma once

#include "max/board/side.h"
#include "max/board/loc.h"
#include "max/board/piececode.h"
#include <stdint.h>

/// \ingroup board
/// @{

/// \defgroup zobrist Zobrist Keys
/// @{


/// A hash key that uniquely identifies a chess position, including factors like en passant availability and
/// castle rights.
/// These keys are used to efficiently compute draws by threefold repetition, and in the chess engine, we use this
/// hash key to index a transposition table array - see the engine transposition table section for more on this topic.
/// @{

#ifdef MAX_ZOBRIST_64

typedef uint64_t max_zobrist_t;

#else

typedef uint32_t max_zobrist_t;

#endif

/// @}

/// Static arrays used to iteratively compute zobrist hash keys
typedef struct {
    max_zobrist_t position[MAX_SIDES_LEN][MAX_PIECEINDEX_LEN][MAX_6BIT_LEN];
    max_zobrist_t castlerights[4];
    max_zobrist_t en_passant_file[8];
} max_zobrist_elements_t;

/// @}

/// @}
