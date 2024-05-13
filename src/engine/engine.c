#include "max/engine/engine.h"
#include "max/board/movegen.h"
#include "max/board/zobrist.h"
#include "max/engine/eval.h"
#include "max/engine/score.h"
#include "private/engine/eval.h"
#include "private/engine/tt.h"


void max_engine_new(max_engine_t *engine, max_engine_init_params_t *init) {
    MAX_ASSERT(init->board.capacity >= 3 & "Board state stack must be at least 3");
    max_board_new(&engine->board, init->board.stack, MAX_ZOBRIST_DEFAULT_SEED);
    max_ttbl_new(&engine->table, init->ttbl.buf, init->ttbl.nbit);
    max_movelist_new(&engine->moves, init->moves.buf, init->moves.capacity);
}

max_score_t max_engine_negamax(max_engine_t *engine, max_movelist_t moves, max_score_t alpha, max_score_t beta, uint8_t depth) {
    if(depth == 0) {
        return max_engine_eval(engine);
        //return max_engine_quiesce(engine, moves, 3);
    }

    max_nodescore_t score = (max_nodescore_t){
        .score = alpha,
        .kind = MAX_NODEKIND_CUT,
        .depth = depth,
    };

    max_board_movegen(&engine->board, &moves);
    for(unsigned i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        if(!max_board_legal(&engine->board, move)) {
            continue;
        }

        max_board_make_move(&engine->board, move);
        max_score_t node = -max_engine_negamax(engine, max_movelist_slice(&moves), -beta, -alpha, depth - 1);
        max_board_unmake_move(&engine->board, move);

        if(node > beta) {
            score.score = node;
            score.kind = MAX_NODEKIND_CUT;
            break;
        }

        if(node > alpha) {
            alpha = node;
            score.score = node;
            score.kind = MAX_NODEKIND_PV;
        }
    }

    return score.score;
}

void max_engine_search(max_engine_t *engine, max_search_result_t *search) {
    search->score = MAX_SCORE_LOWEST;
    max_movelist_t moves = max_movelist_slice(&engine->moves);

    max_board_movegen(&engine->board, &moves);

    for(unsigned i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        if(!max_board_legal(&engine->board, move)) {
            continue;
        }

        max_board_make_move(&engine->board, move);
        max_score_t node = -max_engine_negamax(
            engine,
            max_movelist_slice(&moves),
            MAX_SCORE_LOWEST,
            MAX_SCORE_HIGHEST,
            4
        );
        max_board_unmake_move(&engine->board, move);

        if(node > search->score) {
            search->best = move;
            search->score = node;
        }
    }
}
