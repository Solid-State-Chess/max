/// \file material.h
#pragma once


#include "max/board/piecelist.h"
#include "max/def.h"
#include "max/engine/score.h"


/// \ingroup engine
/// @{

/// \ingroup eval
/// @{

/// \defgroup material Material Scoring
/// Lookup tables used to score the basic value of each chess piece, to be augmented by more complex analysis.
/// @{

/// Configuration for the material values in centipawns of each chess piece.
/// Data can be indexed by the name of the piece type or #max_pieceindex_t.
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

/// Get sensible default values for material scores of each chess piece.
/// \return #max_engine_material_cfg_t
MAX_INLINE_ALWAYS max_engine_material_cfg_t max_engine_material_cfg_default(void) {
    return (max_engine_material_cfg_t){
        .pawn = 100,
        .knight = 300,
        .bishop = 300,
        .rook = 500,
        .queen = 1000,
    };
}

/// Get a value for raw material for the given side measured by the engine's configured piece material scores.
/// \param [in] material Pointer to a structure with configured material scores for each piece type
/// \param [in] pieces A collection of piece lists for each chess piece type
/// \return The sum of material scores for all pieces in the given collection
MAX_INLINE_ALWAYS max_score_t max_engine_score_material(max_engine_material_cfg_t const *material, max_pieces_t *pieces) {
    return 
        pieces->pawn.len   * material->pawn +
        pieces->knight.len * material->knight +
        pieces->bishop.len * material->bishop +
        pieces->rook.len   * material->rook +
        pieces->queen.len  * material->queen;
}

/// @}

/// @}

/// @}
