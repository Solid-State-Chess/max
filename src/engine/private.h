#pragma once

#include "max/engine.h"
#include "max/piece.h"
#include <stdint.h>

#define MAX_PAWN_VALUE    (100)
#define MAX_KNIGHT_VALUE  (300)
#define MAX_BISHOP_VALUE  (310)
#define MAX_ROOK_VALUE    (500)
#define MAX_QUEEN_VALUE   (900)
#define MAX_KING_VALUE    (100000)


/// Get the score in centipawns associated with the given piece type (irrespective of piece colo)
MAX_INLINE_ALWAYS max_score_t max_piecevalue(max_engine_t *engine, max_piececode_t piece) {
    static const max_score_t PIECE_SCORE[32] = {
        [MAX_PIECECODE_EMPTY] = 0,
        [MAX_PIECECODE_PAWN]  = MAX_PAWN_VALUE,
        [MAX_PIECECODE_KNIGHT] = MAX_KNIGHT_VALUE,
        [MAX_PIECECODE_BISHOP] = MAX_BISHOP_VALUE,
        [MAX_PIECECODE_ROOK]   = MAX_ROOK_VALUE,
        [MAX_PIECECODE_QUEEN]  = MAX_QUEEN_VALUE,
        [MAX_PIECECODE_KING]   = MAX_KING_VALUE,
    };

    return PIECE_SCORE[piece & MAX_PIECECODE_TYPE_MASK];
}

/// Perform an alpha-beta search to score the current state of the chessboard
max_score_t max_alpha_beta(max_engine_t *engine, max_score_t alpha, max_score_t beta, uint16_t move_head, uint8_t depth);

/// Evaluate the current position
max_score_t max_evaluate(max_engine_t *engine);

/// Sort moves by a heuristic in order to encourage best moves to be evaulated first
void max_engine_sortmoves(max_engine_t *engine, max_movelist_t *moves);
