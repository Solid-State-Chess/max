#pragma once
/// \file
/// Definitions for incrementally updated zobrist hashes and the lookup tables required to build them

#include "max/board/piece.h"
#include "max/board/square.h"
#include <stdint.h>

/// Zobrist hash of a position that (relatively) uniquely identifies a position with some extra state like castle rights.
/// The key can be built iteratively during move make and unmake using the exclusive or operation - each time a piece is moved its
/// hash is XORed with the value assigned to the piece at that location
typedef uint32_t max_zobrist_t;

/// State required to be stored alongside a board, used to initialize zobrist hash tables and
/// to iteratively update a hash key for the board.
typedef struct {
    /// Incrementally updated hash code.
    max_zobrist_t hash;
    /// Variables used for randomly generated numbers
    uint64_t a,b,c,d;
} max_zobrist_state_t;

/// Zobrist elements assigned for a piece at any of the 64 squares.
/// Utilizing this array directly is not recommended ; better to use max_zobrist_piecepos()
extern max_zobrist_t MAX_ZOBRIST_PIECEPOS[12][64];

/// Zobrist elements assigned to the castle rights bits from a #max_plyplate_t
extern max_zobrist_t MAX_ZOBRIST_CASTLERIGHTS[16];

/// Zobrist elements representing the current en passant file
extern max_zobrist_t MAX_ZOBRIST_EPFILE[8];

/// Get a zobrist hash element for the given piece on the given square. This 
/// element must be combined into the hash key of the board in order to build an identifier for the
/// specific arrangement of pieces.
MAX_INLINE_ALWAYS max_zobrist_t max_zobrist_piecepos(max_piececode_t piece, max_bpos_t pos) {
    static const uint8_t INDEX_BY_PIECETYPE[MAX_PIECECODE_KING + 1] = {
        [MAX_PIECECODE_PAWN]   = 0,
        [MAX_PIECECODE_KNIGHT] = 1,
        [MAX_PIECECODE_BISHOP] = 2,
        [MAX_PIECECODE_ROOK]   = 3,
        [MAX_PIECECODE_QUEEN]  = 4,
        [MAX_PIECECODE_KING]   = 5
    };
    
    uint8_t offset = ((piece & MAX_PIECECODE_BLACK) >> MAX_PIECECODE_BLACK_OFFSET) * 6;
    uint8_t index = INDEX_BY_PIECETYPE[piece & MAX_PIECECODE_TYPE_MASK] + offset;
    return MAX_ZOBRIST_PIECEPOS[index][max_bpos_compress(pos)];
}
