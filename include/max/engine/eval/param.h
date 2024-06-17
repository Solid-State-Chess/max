/// \file param.h
#pragma once


#include "max/engine/eval/material.h"
#include "max/engine/eval/pstbl.h"

/// \ingroup eval
/// @{

/// Evaluation parameters for the engine including 
typedef struct {
    /// Scores assigned to each piece type 
    max_engine_material_cfg_t material;

    max_position_param_t position;
} max_eval_params_t;

/// Get sensible defaults for all evaluation parameters.
MAX_INLINE_ALWAYS max_eval_params_t max_eval_params_default(void) {
    return (max_eval_params_t){
        .material = max_engine_material_cfg_default(),
        .position = max_position_param_default(),
    };
}

/// @}
