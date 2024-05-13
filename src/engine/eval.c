#include "max/engine/eval.h"
#include "max/board/side.h"
#include "private/engine/eval.h"

static max_score_t MAX_SCOREMUL[MAX_SIDES_LEN] = {
    [MAX_SIDE_WHITE] = 1,
    [MAX_SIDE_BLACK] = -1
};


max_score_t max_engine_eval(max_engine_t *engine) {
    max_score_t score = 0;
    
    score +=
        max_engine_score_material(engine, &engine->board.side.white) - 
        max_engine_score_material(engine, &engine->board.side.black);
    return score * MAX_SCOREMUL[max_board_side(&engine->board)];
}
