/// \file eval.h
#pragma once
#include "max/engine/engine.h"
#include "max/engine/score.h"

/// \ingroup engine
/// @{

/// \defgroup eval Evaluation
/// Definitions for lookup tables and other configurable parameters used in move evaluation.
/// @{

/// Evaluate the current position of the board stored in the given engine by a group of
/// basic heuristics.
/// This is used to score positions when searching a game tree in the engine.
max_score_t max_engine_eval(max_engine_t *engine);

/// @}

/// @}
