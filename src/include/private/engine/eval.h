/// \file eval.h
#pragma once
#include "max/board/move.h"
#include "max/def.h"
#include "max/engine/eval.h"
#include "max/engine/score.h"


/// \ingroup eval
/// @{

/// Perform negamax search of the current engine node, returning the best possible score for the side to play.
max_score_t max_engine_negamax(
    max_engine_t *engine,
    max_movelist_t moves,
    max_score_t alpha,
    max_score_t beta,
    uint8_t depth
);

/// Perform quiescence search to stabilize the results of a negamax search, ensuring that there are no obvious captures
/// or checks available.
max_score_t max_engine_quiesce(max_engine_t *engine, max_movelist_t moves, uint8_t depth);

/// Score the material for one side of the chessboard according to the piece tables configured in the engine.
MAX_INLINE_ALWAYS max_score_t max_engine_score_material(max_engine_t *engine, max_pieces_t *pieces) {
    return 
        pieces->pawn.len * engine->param.material.pawn +
        pieces->knight.len * engine->param.material.knight +
        pieces->bishop.len * engine->param.material.bishop +
        pieces->rook.len * engine->param.material.rook +
        pieces->queen.len * engine->param.material.queen;
}

/// @}
