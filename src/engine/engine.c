#include "max/engine/engine.h"
#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/board/state.h"
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
#include <time.h>


void max_engine_new(max_engine_t *engine, max_engine_init_params_t *init, max_eval_params_t param) {
    MAX_ASSERT(init->board.capacity >= 3 && "Board state stack must be at least 3");
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
        return stand;
    }

    max_board_movegen(&engine->board, &moves);
    //max_engine_sortmoves(engine, moves);
    for(unsigned i = 0; i < moves.len; ++i) {
        max_smove_t move = moves.buf[i];
        if((move.tag & MAX_MOVETAG_CAPTURE) != MAX_MOVETAG_CAPTURE || !max_board_legal(&engine->board, move)) {
            continue;
        }

        max_board_make_move(&engine->board, move);
        max_score_t score = -max_engine_quiesce(engine, max_movelist_slice(&moves), -beta, -alpha, depth - 1);
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

    if(probed != NULL && probed->depth >= depth) {
        DIAGNOSTIC(engine->diagnostic.ttbl_hits += 1);
        switch(probed->attr & MAX_TTENTRY_PATTR_KIND_MASK) {
            case (MAX_NODEKIND_PV << MAX_TTENTRY_PATTR_KIND_POS): {
                if(probed->score >= beta) {
                    DIAGNOSTIC(engine->diagnostic.ttbl_used += 1);
                    return beta;
                }

                if(probed->score >= alpha && probed->score < beta) {
                    DIAGNOSTIC(engine->diagnostic.ttbl_used += 1);
                    return probed->score;
                }
            } break;

            case (MAX_NODEKIND_ALL << MAX_TTENTRY_PATTR_KIND_POS): {
                if(probed->score <= alpha) {
                    DIAGNOSTIC(engine->diagnostic.ttbl_used += 1);
                    beta = probed->score;
                    return alpha;
                }
            } break;

            case (MAX_NODEKIND_CUT << MAX_TTENTRY_PATTR_KIND_POS): {
                if(probed->score >= beta) {
                    DIAGNOSTIC(engine->diagnostic.ttbl_used += 1);
                    return beta;
                }
            } break;
        }
    }

    max_nodescore_t score = (max_nodescore_t){
        .score = MAX_SCORE_LOWEST + 20,
        .kind = MAX_NODEKIND_ALL,
        .depth = depth,
    };

    max_board_movegen(&engine->board, &moves);
    max_engine_sortmoves(engine, moves);

    uint8_t legal_count = 0;
    for(unsigned i = 0; i < moves.len; ++i) {
        if(time(NULL) - engine->time >= 25) {
            return alpha;
        }
        max_smove_t move = moves.buf[i];
        if(!max_board_legal(&engine->board, move)) {
            continue;
        }

        legal_count += 1;

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
            score.bestmove = move;
        }

        if(node > alpha) {
            alpha = node;
            score.kind = MAX_NODEKIND_PV;
        }
    }

    if(legal_count == 0) {
        if(!max_check_is_empty(max_board_state(&engine->board)->check[0])) {
            return -20000 - depth;
        } else {
            return -depth;
        }
    }

    max_ttbl_probe_insert(
        &engine->table,
        hash,
        score,
        engine->board.ply
    );

    return score.score;
}

bool max_engine_search_moves(max_engine_t *engine, max_scorelist_t *scored_moves, max_search_result_t *search, uint8_t depth) {
    max_scorelist_sort(scored_moves);
    uint8_t nlegal = 0;
    for(unsigned i = 0; i < scored_moves->moves.len; ++i) {
        if(time(NULL) - engine->time >= 25) {
            return false;
        }
        max_smove_t move = scored_moves->moves.buf[i];
        if(!max_board_legal(&engine->board, move)) {
            continue;
        }

        nlegal += 1;

        max_board_make_move(&engine->board, move);
        max_score_t node = -max_engine_negamax(
            engine,
            max_movelist_slice(&scored_moves->moves),
            MAX_SCORE_LOWEST + 20,
            MAX_SCORE_HIGHEST - 20,
            depth - 1
        );
        printf("%c%c%c%c @ %d - depth %u\n", MAX_0x88_FORMAT(move.from), MAX_0x88_FORMAT(move.to), node, depth);
        max_board_unmake_move(&engine->board, move);
        max_scorelist_score(scored_moves, i, node);

        if(node + depth * 20 > search->score + search->depth * 20 || (move.from.v == search->best.from.v && move.to.v == search->best.to.v)) {
            search->best = move;
            search->score = node;
            search->depth = depth;
        }
    }
    
    return nlegal == 0;
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
    search->gameover = false;
    max_state_stack_lower_head(&engine->board.stack, 3);
    max_movelist_t moves = max_movelist_slice(&engine->moves);

    max_board_movegen(&engine->board, &moves);
    
    max_scorelist_t scored_moves;
    max_scorelist_reset(&scored_moves, moves);

    engine->time = time(NULL);
    for(uint8_t depth = 2; ; depth += 1) {
        if(time(NULL) - engine->time >= 25) {
            return;
        }

        if(max_engine_search_moves(engine, &scored_moves, search, depth)) {
            search->gameover = true;
            break;
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
        best_to.v   = max_ttentry_pattr_dest(probed->attr).v;
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
