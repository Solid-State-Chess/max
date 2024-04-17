#include "max/board/board.h"
#include "max/board/move.h"
#include "max/board/movegen.h"
#include "max/engine/engine.h"
#include "max/engine/tt.h"
#include "private.h"
#include <stdint.h>

#define MAX_TIME 25

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

    if(engine->board.stack.plies_since_reset >= (MAX_ENGINE_MAX_PLY - 1)) {
        return standing;
    }

    max_movelist_t captures = max_movelist_new(engine->search.moves + move_head);
    max_board_capturegen_pseudo(&engine->board, &captures);
    max_engine_sortmoves(engine, &captures);
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

max_score_t max_alpha_beta(
    max_engine_t *engine,
    max_score_t alpha,
    max_score_t beta,
    uint16_t move_head,
    uint8_t depth
) {
    if(depth == 0) {
        return max_quiesce(engine, alpha, beta, move_head);
    } else {
        max_score_t local_alpha = INT16_MIN;
        max_move_t *best = NULL;
        max_ttentry_t *record;
        if((record = max_ttbl_probe(&engine->tt, engine->board.zobrist.hash)) != NULL) {
            switch(record->attr.node_type) {
                case MAX_TTENTRY_TYPE_PV: {
                    if(record->attr.depth >= depth && record->score >= alpha && record->score < beta) {
                        engine->diagnostic.tt_hits += 1;
                        alpha = record->score;
                        return record->score;
                    }
                } break;
                case MAX_TTENTRY_TYPE_UPPER: {
                    if(record->attr.depth >= depth && record->score <= alpha) {
                        engine->diagnostic.tt_hits += 1;
                        return alpha;
                    }
                } break;
                case MAX_TTENTRY_TYPE_LOWER: {
                    if(record->attr.depth >= depth && record->score > alpha && record->score >= beta) {
                        engine->diagnostic.tt_hits += 1;
                        alpha = record->score;
                        return beta;
                    }
                } break;
            }
        }

        record = max_ttbl_slot(&engine->tt, engine->board.zobrist.hash, engine->board.ply, depth);


        max_movelist_t moves = max_movelist_new(engine->search.moves + move_head);
        max_board_movegen_pseudo(&engine->board, &moves);
        max_engine_sortmoves(engine, &moves);
        
        uint8_t movecount = 0;
        
        for(uint8_t i = 0; i < moves.len; ++i) {
            max_move_t move = moves.moves[i];
            if(!max_board_move_is_valid(&engine->board, move)) {
                continue;
            }

            max_board_make_move(&engine->board, move);
            movecount += 1;
            max_score_t score = -max_alpha_beta(engine, -beta, -alpha, move_head + moves.len, depth - 1);
            max_board_unmake_move(&engine->board, move);
            
            if(score >= beta) {
                if(record) {
                    *record = (max_ttentry_t){
                        .key_part = max_ttbl_extract_key(engine->board.zobrist.hash),
                        .score = score,
                        .move = move,
                        .attr = (max_ttentry_attr_t){
                            .age = engine->board.ply,
                            .depth = depth,
                            .gravestone = false,
                            .node_type = MAX_TTENTRY_TYPE_LOWER,
                        }
                    };
                } 
                return beta;
            }
            if(score > alpha) {
                alpha = score;
            }

            if(score > local_alpha) {
                local_alpha = score;
                best = &moves.moves[i];
            }

            if(time(NULL) - engine->start > MAX_TIME) {
                return alpha; 
            }

        }
        
        if(movecount > 0) {
            if(record) {
                if(local_alpha >= alpha) {
                    *record = (max_ttentry_t){
                        .key_part = max_ttbl_extract_key(engine->board.zobrist.hash),
                        .score = alpha,
                        .move = *best,
                        .attr = (max_ttentry_attr_t){
                            .age = engine->board.ply,
                            .depth = depth,
                            .gravestone = false,
                            .node_type = MAX_TTENTRY_TYPE_PV,
                        }
                    };
                } else {
                    *record = (max_ttentry_t){
                        .key_part = max_ttbl_extract_key(engine->board.zobrist.hash),
                        .score = local_alpha,
                        .move = *best,
                        .attr = (max_ttentry_attr_t){
                            .age = engine->board.ply,
                            .depth = depth,
                            .gravestone = false,
                            .node_type = MAX_TTENTRY_TYPE_UPPER,
                        }
                    };
                }
            }

            return alpha;
        } else if(max_check_exists(max_board_state(&engine->board)->check)) {
            return -(MAX_KING_VALUE + depth);
        } else {
            return -MAX_KING_VALUE / 2;
        }
    }
}

typedef struct {
    max_score_t score;
    uint8_t depth;
} movescore_t;

static bool max_engine_search_part(max_engine_t *engine, max_movelist_t moves, movescore_t *buf, uint8_t depth) {
    max_board_reset_stack(&engine->board);

    bool checkmate = true;
    for(uint8_t i = 0; i < moves.len; ++i) {
        if(time(NULL) - engine->start > MAX_TIME) {
            return false; 
        }

        if(!max_board_move_is_valid(&engine->board, moves.moves[i])) {
            continue;
        }
        checkmate = false;

        max_board_make_move(&engine->board, moves.moves[i]);
        max_score_t score = -max_alpha_beta(engine, INT16_MIN + 20, INT16_MAX - 20, moves.len, depth);
        max_board_unmake_move(&engine->board, moves.moves[i]);
        buf[i].score = score;
        buf[i].depth = depth;
    }

    return checkmate;
}


bool max_engine_search(max_engine_t *engine, max_searchresult_t *search) {
    engine->diagnostic.nodes = 0;
    engine->diagnostic.tt_hits = 0;
    engine->start = time(NULL);

    max_searchresult_t tmp;

    max_movelist_t moves = max_movelist_new(engine->search.moves);
    max_board_movegen_pseudo(&engine->board, &moves);
    
    static movescore_t buf[2048];

    uint8_t depth = 1;
    while(time(NULL) - engine->start <= MAX_TIME) {
        if(max_engine_search_part(engine, moves, buf, depth)) {
            return false;
        }

        depth += 1;
    }

    uint8_t best = 0;
    for(unsigned i = 0; i < moves.len; ++i) {
        if(max_board_move_is_valid(&engine->board, moves.moves[i])) {
            if(buf[i].score > buf[best].score + (30 * ((int8_t)(buf[i].depth) - (int8_t)(buf[best].depth)))) {
                best = i;
            }
        }
    }

    search->best_score = buf[best].score;
    search->bestmove = moves.moves[best];
    search->depth = buf[best].depth;

    return true;
}
