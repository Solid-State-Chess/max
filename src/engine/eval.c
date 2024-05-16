#include "max/engine/eval.h"
#include "max/board/piececode.h"
#include "max/board/side.h"
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

static max_score_t max_evaluate_slider_control(max_engine_t *engine, max_pieces_t *side) {
    max_score_t count = 0;
    for(unsigned i = 0; i < side->bishop.len; ++i) {
        count += max_count_squares(engine, side->bishop.loc[i], MAX_0x88_DIR_UR);
        count += max_count_squares(engine, side->bishop.loc[i], MAX_0x88_DIR_UL);
        count += max_count_squares(engine, side->bishop.loc[i], MAX_0x88_DIR_DR);
        count += max_count_squares(engine, side->bishop.loc[i], MAX_0x88_DIR_DL);
    }

    for(unsigned i = 0; i < side->rook.len; ++i) {
        count += max_count_squares(engine, side->rook.loc[i], MAX_0x88_DIR_UP);
        count += max_count_squares(engine, side->rook.loc[i], MAX_0x88_DIR_DOWN);
        count += max_count_squares(engine, side->rook.loc[i], MAX_0x88_DIR_LEFT);
        count += max_count_squares(engine, side->rook.loc[i], MAX_0x88_DIR_RIGHT);
    }

    for(unsigned i = 0; i < side->queen.len; ++i) {
        count += max_count_squares(engine, side->queen.loc[i], MAX_0x88_DIR_UR);
        count += max_count_squares(engine, side->queen.loc[i], MAX_0x88_DIR_UL);
        count += max_count_squares(engine, side->queen.loc[i], MAX_0x88_DIR_DR);
        count += max_count_squares(engine, side->queen.loc[i], MAX_0x88_DIR_DL);
        count += max_count_squares(engine, side->queen.loc[i], MAX_0x88_DIR_UP);
        count += max_count_squares(engine, side->queen.loc[i], MAX_0x88_DIR_DOWN);
        count += max_count_squares(engine, side->queen.loc[i], MAX_0x88_DIR_LEFT);
        count += max_count_squares(engine, side->queen.loc[i], MAX_0x88_DIR_RIGHT);
    }

    return count * 7;
}

max_score_t max_engine_eval(max_engine_t *engine) {
    max_score_t score = 0;
    
    score +=
        max_engine_score_material(engine, &engine->board.side.white) - 
        max_engine_score_material(engine, &engine->board.side.black);

    score +=
        max_engine_score_positions(engine, &engine->board.side.white, MAX_SIDE_WHITE) -
        max_engine_score_positions(engine, &engine->board.side.black, MAX_SIDE_BLACK);

    score +=
        max_evaluate_slider_control(engine, &engine->board.side.white) -
        max_evaluate_slider_control(engine, &engine->board.side.black);
    
    DIAGNOSTIC(engine->diagnostic.nodes += 1);

    return score * MAX_SCOREMUL[max_board_side(&engine->board)];
}
