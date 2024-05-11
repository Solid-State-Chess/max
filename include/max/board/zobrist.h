/// \file zobrist.h

#pragma once

#include "max/board/side.h"
#include "max/board/loc.h"
#include "max/board/piececode.h"
#include <stdint.h>

/// \ingroup board
/// @{

/// \defgroup zobrist Zobrist Keys
/// Zobrist hashing builds unique identifiers for chess positions, to be used in a transposition table and to
/// efficiently store and compare positions when determining draws by threefold repetition.
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

/// Static arrays used to incrementally compute zobrist hash keys.
/// These contribute the largest memory footprint to the board by itself,
/// but allow for huge performance gains in engine processing and threefold repetition.
typedef struct {
    /// An array indexed by the side that a piece is on, the piece type, and the position of the piece.
    /// Zobrist hashes are internally built by XORing the current hash with this whenever a piece is added to or
    /// removed from the board
    max_zobrist_t position[MAX_SIDES_LEN][MAX_PIECEINDEX_LEN][MAX_6BIT_LEN];
    /// Castle rights array representing the ABSENCE of castle rights for white and black's
    /// king and queenside rooks.
    max_zobrist_t castlerights[4];
    /// Elements added to the final hash if en passant is possible on a file, otherwise no extra
    /// element is added
    max_zobrist_t en_passant_file[8];
} max_zobrist_elements_t;

/// Default seed for the random number generator used to create zobrist hash elements when creating a board.
#define MAX_ZOBRIST_DEFAULT_SEED (0xfa3198db566d5520)

/// @}

/// @}
