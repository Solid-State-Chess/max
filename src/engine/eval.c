#include "max/engine/eval.h"
#include "max/board/side.h"

static max_score_t MAX_SCOREMUL[MAX_SIDES_LEN] = {
    [MAX_SIDE_WHITE] = 1,
    [MAX_SIDE_BLACK] = -1
};

max_score_t max_engine_eval(max_engine_t *engine) {
    max_score_t score = 0;
    
    score += engine->board.side.white.pawn.len * 100;
    score -= engine->board.side.black.pawn.len * 100;
    return score * MAX_SCOREMUL[max_board_side(&engine->board)];
}
