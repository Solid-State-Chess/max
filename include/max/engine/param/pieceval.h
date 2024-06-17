/// \file pieceval.h
#pragma once

#include "max/def.h"
#include "max/engine/score.h"

/// \ingroup param
/// @{

/// Configuration for the material values in centipawns of each chess piece
typedef union {
    struct {
        max_score_t pawn;
        max_score_t knight;
        max_score_t bishop;
        max_score_t rook;
        max_score_t queen;
    };
    
    max_score_t array[MAX_PIECEINDEX_LEN];
} max_engine_material_cfg_t;

/// Get sensible default values for material
MAX_INLINE_ALWAYS max_engine_material_cfg_t max_engine_material_cfg_default(void) {
    return (max_engine_material_cfg_t){
        .pawn = 100,
        .knight = 300,
        .bishop = 300,
        .rook = 500,
        .queen = 1000,
    };
}

/// @}
