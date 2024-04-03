#include "max/board.h"
#include "max/engine.h"
#include "private.h"
#include <stdint.h>

max_score_t max_alpha_beta(max_engine_t *engine, max_score_t alpha, max_score_t beta, uint16_t move_head, uint8_t depth) {
    if(depth == 0) {
        return max_evaluate(engine);
    } else {
        max_score_t max = (max_score_t)(INT32_MAX);
        max_movelist_t moves = max_movelist_new(engine->search.moves + move_head);
        max_board_movegen_pseudo(&engine->board, &moves);

        for(uint8_t i = 0; i < moves.len; ++i) {
            max_board_make_move(&engine->board, moves.moves[i]);

            max_score_t score = -max_alpha_beta(engine, -beta, -alpha, move_head + moves.len, depth - 1);
            max_board_unmake_move(&engine->board, moves.moves[i]);
            if(score >= beta) {
                return beta;
            }
            if(score > alpha) {
                alpha = score;
            }
        }

        return alpha;
    }
}


void max_engine_search(max_engine_t *engine, max_searchresult_t *search, uint8_t depth) {
    max_score_t bestmove;
    max_movelist_t moves = max_movelist_new(engine->search.moves);
    max_board_movegen_pseudo(&engine->board, &moves);

    search->best_score = INT32_MIN;

    for(uint8_t i = 0; i < moves.len; ++i) {
        max_board_make_move(&engine->board, moves.moves[i]);

        max_score_t score = max_alpha_beta(engine, INT32_MIN + 20, INT32_MAX - 20, moves.len, depth);

        printf("%X %X - %i\n", moves.moves[i].from, moves.moves[i].to, score);

        max_board_unmake_move(&engine->board, moves.moves[i]);
        if(score > search->best_score) {
            search->best_score = score;
            search->bestmove = moves.moves[i];
        }
    }
}
