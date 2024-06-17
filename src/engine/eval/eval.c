#include "max/engine/eval/eval.h"
#include "max/board/piececode.h"
#include "max/board/side.h"
#include "max/engine/eval/material.h"
#include "private/engine/engine.h"
#include "private/engine/eval.h"

static max_score_t MAX_SCOREMUL[MAX_SIDES_LEN] = {
    [MAX_SIDE_WHITE] = 1,
    [MAX_SIDE_BLACK] = -1
};

static max_score_t max_count_squares(max_engine_t *engine, max_0x88_t pos, max_0x88_dir_t dir) {
    max_score_t count = 0;
    for(;;) {
        pos = max_0x88_move(pos, dir);
        if(!max_0x88_valid(pos)) {
            return count;
        }

        count += 1;

        max_piececode_t piece = engine->board.pieces[pos.v];
        if(piece.v != MAX_PIECECODE_EMPTY) {
            return count;
        }
    }
}

max_score_t max_engine_eval(max_engine_t *engine) {
    max_score_t score = 0;
    
    score +=
        max_engine_score_material(&engine->param.material, &engine->board.side.white) - 
        max_engine_score_material(&engine->param.material, &engine->board.side.black);

    score +=
        max_engine_score_positions(engine, &engine->board.side.white, MAX_SIDE_WHITE) -
        max_engine_score_positions(engine, &engine->board.side.black, MAX_SIDE_BLACK);

    DIAGNOSTIC(engine->diagnostic.nodes += 1);

    return score * MAX_SCOREMUL[max_board_side(&engine->board)];
}
