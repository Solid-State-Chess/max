/// \file param.h
#pragma once
#include "max/def.h"
#include "max/engine/param/pieceval.h"
#include "max/engine/param/pstbl.h"


/// \ingroup engine
/// @{

/// \defgroup param Engine Parameters
/// Configurable engine parameters to be used in evaluation
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

/// @}
