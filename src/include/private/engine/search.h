#pragma once


#include "max/engine/engine.h"
#include "max/engine/score.h"

void max_engine_sort();

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
max_score_t max_engine_quiesce(max_engine_t *engine, max_movelist_t moves, max_score_t alpha, max_score_t beta, uint8_t depth);
