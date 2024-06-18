/// \file param.h
#pragma once


#include "max/engine/eval/material.h"
#include "max/engine/eval/pstbl.h"

/// \ingroup eval
/// @{

/// A collection of all engine evaluation parameters used to tune the engine's evaluation performance.
typedef struct {
    /// Scores assigned to each piece type that provide the majority of evaluation, the rest augments this to
    /// discriminate between more equal positions only separated by more strategic analysis.
    max_engine_material_cfg_t material;
    /// Piece-square tables for the mid and endgame that encourage basic positional play by scoring certain squares
    /// higher.
    max_engine_psqt_param_t position;
} max_eval_params_t;

/// Get sensible defaults for all evaluation parameters.
/// \see max_engine_material_cfg_default()
/// \see max_position_param_default()
/// \return #max_eval_params_t with all defaults
MAX_INLINE_ALWAYS max_eval_params_t max_eval_params_default(void) {
    return (max_eval_params_t){
        .material = max_engine_material_cfg_default(),
        .position = max_engine_psqt_param_default(),
    };
}

/// @}
