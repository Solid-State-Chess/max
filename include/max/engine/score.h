#pragma once

#include <stdint.h>
#include "max/board/move.h"

/// A signed score as the result of a board evaluation measured in centipawns.
typedef int16_t max_score_t;

#define MAX_ENGINE_MAX_MOVESCORES (56)

/// A list of moves that have each been scored with a search by the engine.
/// Scores and depth of analysis are both stored with the same indices as the move list..
typedef struct {
    max_move_t moves[MAX_ENGINE_MAX_MOVESCORES];
    max_score_t scores[MAX_ENGINE_MAX_MOVESCORES];
    uint8_t depths[MAX_ENGINE_MAX_MOVESCORES];
    uint8_t len;
} max_scored_movelist_t;
