#pragma once

#include "max/engine.h"
#include <stdint.h>

/// Perform an alpha-beta search to score the current state of the chessboard
max_score_t max_alpha_beta(max_engine_t *engine, max_score_t alpha, max_score_t beta, uint16_t move_head, uint8_t depth);

/// Evaluate the current position
max_score_t max_evaluate(max_engine_t *engine);
