#include "max/engine/engine.h"
#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/squares.h"
#include "max/board/zobrist.h"
#include "max/engine/eval.h"
#include "max/engine/score.h"
#include "max/engine/tt.h"
#include "private/board/board.h"
#include "private/board/state.h"
#include "private/engine/engine.h"
#include "private/engine/eval.h"
#include "private/engine/search.h"
#include "private/engine/tt.h"
#include <stdio.h>
#include <stdlib.h>


void max_engine_new(max_engine_t *engine, max_engine_init_params_t *init, max_eval_params_t param) {
    MAX_ASSERT(init->board.capacity >= 3 & "Board state stack must be at least 3");
    max_board_new(&engine->board, init->board.stack, MAX_ZOBRIST_DEFAULT_SEED);
    max_ttbl_new(&engine->table, init->ttbl.buf, init->ttbl.nbit);
    max_movelist_new(&engine->moves, init->moves.buf, init->moves.capacity);
    engine->param = param;
}

max_score_t max_engine_quiesce(max_engine_t *engine, max_movelist_t moves, max_score_t alpha, max_score_t beta, uint8_t depth) {
    max_score_t stand = max_engine_eval(engine);
    if(stand >= beta) {
        return beta;
    }

    if(stand > alpha) {
        alpha = stand;
    }

    if(depth == 0) {
        return alpha;
    }

    max_board_movegen(&engine->board, &moves);
    max_engine_sortmoves(engine, moves);
    for(unsigned i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        if((move.tag & MAX_MOVETAG_CAPTURE) != MAX_MOVETAG_CAPTURE || !max_board_legal(&engine->board, move)) {
            continue;
        }

        max_board_make_move(&engine->board, move);
        max_score_t score = -max_engine_quiesce(engine, max_movelist_slice(&moves), -alpha, -beta, depth - 1);
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

max_score_t max_engine_negamax(max_engine_t *engine, max_movelist_t moves, max_score_t alpha, max_score_t beta, uint8_t depth) {
    if(depth == 0) {
        return max_engine_quiesce(engine, moves, alpha, beta, 3);
    }

    max_zobrist_t hash = max_board_state(&engine->board)->position;
    max_ttentry_t const *probed = max_ttbl_probe_read(&engine->table, hash);

    if(probed != NULL && max_ttentry_pattr_depth(probed->attr) >= depth) {
        DIAGNOSTIC(engine->diagnostic.ttbl_hits += 1);
        switch(probed->attr & MAX_TTENTRY_PATTR_KIND_MASK) {
            case (MAX_NODEKIND_PV << MAX_TTENTRY_PATTR_KIND_POS): {
                if(probed->score > beta) {
                    DIAGNOSTIC(engine->diagnostic.ttbl_used += 1);
                    return beta;
                }

                if(probed->score > alpha) {
                    DIAGNOSTIC(engine->diagnostic.ttbl_used += 1);
                    return probed->score;
                }
            } break;

            case (MAX_NODEKIND_ALL << MAX_TTENTRY_PATTR_KIND_POS): {
                if(probed->score < beta) {
                    DIAGNOSTIC(engine->diagnostic.ttbl_used += 1);
                    beta = probed->score;
                }
            } break;

            case (MAX_NODEKIND_CUT << MAX_TTENTRY_PATTR_KIND_POS): {
                if(probed->score > beta) {
                    DIAGNOSTIC(engine->diagnostic.ttbl_used += 1);
                    return beta;
                }
            } break;
        }
    }

    max_nodescore_t score = (max_nodescore_t){
        .score = MAX_SCORE_LOWEST,
        .kind = MAX_NODEKIND_ALL,
        .depth = depth,
    };

    max_board_movegen(&engine->board, &moves);
    max_engine_sortmoves(engine, moves);
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
            score.bestmove = move;
            break;
        }

        if(node > score.score) {
            score.score = node;
            score.kind = MAX_NODEKIND_ALL;
            score.bestmove = move;
        }

        if(node > alpha) {
            alpha = node;
            score.score = node;
            score.kind = MAX_NODEKIND_PV;
            score.bestmove = move;
        }
    }

    max_ttbl_probe_insert(
        &engine->table,
        hash,
        score
    );

    return score.score;
}

void max_engine_search(max_engine_t *engine, max_search_result_t *search) {
    DIAGNOSTIC(
        engine->diagnostic = (max_engine_diagnostic_t){
            .nodes = 0,
            .ttbl_hits = 0,
            .ttbl_used = 0,
        }
    );

    search->score = MAX_SCORE_LOWEST;
    max_state_stack_lower_head(&engine->board.stack, 3);
    max_movelist_t moves = max_movelist_slice(&engine->moves);

    max_board_movegen(&engine->board, &moves);
    max_engine_sortmoves(engine, moves);

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
            5
        );
        max_board_unmake_move(&engine->board, move);

        if(node > search->score) {
            search->best = move;
            search->score = node;
        }
    }
}


void max_engine_sortmoves(max_engine_t *engine, max_movelist_t moves) {
    static max_scorelist_t SCORES;
    max_scorelist_reset(&SCORES, moves);
    if(moves.len > MAX_ENGINE_MAX_MOVES_PER_PLY) {
        moves.len = MAX_ENGINE_MAX_MOVES_PER_PLY;
    }
    
    max_0x88_t best_from = max_0x88_raw(MAX_0x88_INVALID_MASK);
    max_0x88_t best_to   = max_0x88_raw(MAX_0x88_INVALID_MASK);
    max_ttentry_t const *probed = max_ttbl_probe_read(&engine->table, max_board_state(&engine->board)->position);
    if(probed != NULL) {
        best_from.v = max_ttentry_pattr_source(probed->attr).v;
        best_to.v   = max_ttentry_pattr_source(probed->attr).v;
    }

    for(uint8_t i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        max_score_t score = 0;

        // Most Valuable Viction - Least Valuable Aggressor scoring
        if(move.tag & MAX_MOVETAG_CAPTURE) {
            max_score_t aggressor = max_engine_score_piece(engine, engine->board.pieces[move.from.v]);
            max_score_t victim    = max_engine_score_piece(engine, engine->board.pieces[move.to.v]);
            score += victim - aggressor;
        }
        
        if(move.from.v == best_from.v && move.to.v == best_to.v) {
            score += 1000;
        }

        max_scorelist_score(&SCORES, i, score);
    }

    max_scorelist_sort(&SCORES);
}
