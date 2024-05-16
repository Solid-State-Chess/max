/// \file eval.h
#pragma once
#include "max/board/loc.h"
#include "max/board/piececode.h"
#include "max/board/piecelist.h"
#include "max/def.h"
#include "max/engine/engine.h"
#include "max/engine/param/pstbl.h"
#include "max/engine/score.h"


/// \ingroup eval
/// @{


/// Score the material for one side of the chessboard according to the piece tables configured in the engine.
MAX_INLINE_ALWAYS max_score_t max_engine_score_material(max_engine_t *engine, max_pieces_t *pieces) {
    return 
        pieces->pawn.len * engine->param.material.pawn +
        pieces->knight.len * engine->param.material.knight +
        pieces->bishop.len * engine->param.material.bishop +
        pieces->rook.len * engine->param.material.rook +
        pieces->queen.len * engine->param.material.queen;
}

MAX_INLINE_ALWAYS max_score_t max_engine_score_positions_single(max_pstbl_t tbl, max_loclist_t *list, max_side_t side) {
    max_score_t score = 0;
    for(unsigned i = 0; i < list->len; ++i) {
        max_0x88_t pos = list->loc[i];
        if(side == MAX_SIDE_WHITE) {
            pos = max_0x88_mirror_y(pos);
        }
        
        score += tbl[max_0x88_to_6bit(pos).v];
    }

    return score;
}

MAX_INLINE_ALWAYS max_score_t max_engine_score_positions(max_engine_t *engine, max_pieces_t *pieces, max_side_t side) {
    return
        max_engine_score_positions_single(engine->param.position.pawn, (max_loclist_t*)(&pieces->pawn), side) +
        max_engine_score_positions_single(engine->param.position.knight, (max_loclist_t*)(&pieces->knight), side) +
        max_engine_score_positions_single(engine->param.position.bishop, (max_loclist_t*)(&pieces->bishop), side) +
        max_engine_score_positions_single(engine->param.position.rook, (max_loclist_t*)(&pieces->rook), side) +
        max_engine_score_positions_single(engine->param.position.queen, (max_loclist_t*)(&pieces->queen), side) +
        max_engine_score_positions_single(engine->param.position.king, (max_loclist_t*)(&pieces->king), side);
}

/// Get a score for the given piececode, regardless of the side that is played
MAX_INLINE_ALWAYS max_score_t max_engine_score_piece(max_engine_t *engine, max_piececode_t piece) {
    return engine->param.material.array[max_piececode_kind_index(piece)];
}

/// @}
