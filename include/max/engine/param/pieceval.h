/// \file pieceval.h
#pragma once

#include "max/def.h"
#include "max/engine/score.h"

/// \ingroup param
/// @{

typedef struct {
    max_score_t pawn;
    max_score_t knight;
    max_score_t bishop;
    max_score_t rook;
    max_score_t queen;
} max_piece_value_t;

/// Get sensible default values for material
MAX_INLINE_ALWAYS max_piece_value_t max_piece_value_default(void) {
    return (max_piece_value_t){
        .pawn = 100,
        .knight = 300,
        .bishop = 300,
        .rook = 500,
        .queen = 1000,
    };
}

/// @}
