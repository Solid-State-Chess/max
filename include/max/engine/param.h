/// \file param.h
#pragma once
#include "max/def.h"
#include "max/engine/param/pieceval.h"


/// \ingroup engine
/// @{

/// \defgroup param Engine Parameters
/// Configurable engine parameters to be used in evaluation
/// @{

/// Evaluation parameters for the engine including 
typedef struct {
    /// Scores assigned to each piece type 
    max_piece_value_t material;
} max_eval_params_t;

/// Get sensible defaults for all evaluation parameters.
MAX_INLINE_ALWAYS max_eval_params_t max_eval_params_default(void) {
    return (max_eval_params_t){
        .material = max_piece_value_default(),
    };
}

/// @}

/// @}
