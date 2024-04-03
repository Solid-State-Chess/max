#pragma once

#include "max/engine.h"
#include <stdint.h>

/// Perform an alpha-beta search to score the current state of the chessboard
max_score_t max_alpha_beta(max_engine_t *engine, uint8_t depth);
