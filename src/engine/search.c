#include "max/board.h"
#include "max/engine.h"
#include "private.h"
#include <stdint.h>

/// Amount to multiply the score of an evaluation by for the side to play
static const max_score_t SCORE_MUL[2] = {1, -1};

/// Perform a quiescence search of the given node
static max_score_t max_quiesce(max_engine_t *engine, max_score_t alpha, max_score_t beta, uint16_t move_head) {
    max_score_t standing = max_evaluate(engine) * SCORE_MUL[engine->board.ply & 1];
    if(standing >= beta) {
        return beta;
    }
    if(standing > alpha) {
        alpha = standing;
    }

    max_movelist_t captures = max_movelist_new(engine->search.moves + move_head);
    max_board_capturegen_pseudo(&engine->board, &captures);
    for(unsigned i = 0; i < captures.len; ++i) {
        max_move_t capture = captures.moves[i];
        if(!max_board_move_is_valid(&engine->board, capture)) {
            continue;
        }

        max_board_make_move(&engine->board, capture);
        max_score_t score = -max_quiesce(engine, -beta, -alpha, move_head + captures.len);
        max_board_unmake_move(&engine->board, capture);

        if(score >= beta) {
            return beta;
        }
        if(score > alpha) {
            alpha = score;
        }
    }
    

    return alpha;
}

max_score_t max_alpha_beta(max_engine_t *engine, max_score_t alpha, max_score_t beta, uint16_t move_head, uint8_t depth) {
    if(depth == 0) {
        return max_quiesce(engine, alpha, beta, move_head);
    } else {
        max_movelist_t moves = max_movelist_new(engine->search.moves + move_head);
        max_board_movegen_pseudo(&engine->board, &moves);
        max_engine_sortmoves(engine, &moves);

        for(uint8_t i = 0; i < moves.len; ++i) {
            max_move_t move = moves.moves[i];
            if(!max_board_move_is_valid(&engine->board, move)) {
                continue;
            }

            max_board_make_move(&engine->board, move);

            max_score_t score = -max_alpha_beta(engine, -beta, -alpha, move_head + moves.len, depth - 1);
            max_board_unmake_move(&engine->board, move);
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
    engine->diagnostic.nodes = 0;
    engine->diagnostic.futility_pruned = 0;
    max_movelist_t moves = max_movelist_new(engine->search.moves);
    max_board_movegen_pseudo(&engine->board, &moves);

    search->best_score = INT32_MIN;

    for(uint8_t i = 0; i < moves.len; ++i) {
        if(!max_board_move_is_valid(&engine->board, moves.moves[i])) {
            continue;
        }

        max_board_make_move(&engine->board, moves.moves[i]);

        max_score_t score = -max_alpha_beta(engine, INT32_MIN + 20, INT32_MAX - 20, moves.len, depth);

        max_board_unmake_move(&engine->board, moves.moves[i]);

        if(score > search->best_score) {
            search->best_score = score;
            search->bestmove = moves.moves[i];
        }
    }
}
