#pragma once

#include "max/board.h"

#define MAX_ENGINE_MAX_PLY (5024)

/// A signed score as the result of a board evaluation measured in centipawns
typedef int32_t max_score_t;

/// State required at all stages of an alpha-beta search
typedef struct {
    max_move_t moves[MAX_ENGINE_MAX_PLY];
    /// Number of nodes searched
    uint32_t nodes;
} max_search_state_t;

/// Complete engine state including lookup tables, settings, and chessboard representation
typedef struct {
    max_board_t board;
    max_search_state_t search;
} max_engine_t;

/// Result of a move search with diagnostic information and selected move
typedef struct {
    /// Move that would result in the best outcome board
    max_move_t bestmove;
    /// Score in centipawns of the selected move
    max_score_t best_score;
} max_searchresult_t;

/// Initialize an engine with an initial chess board position
void max_engine_new(max_engine_t *engine);

/// Search for the best move on this board for the side to play
void max_engine_search(max_engine_t *engine, max_searchresult_t *search, uint8_t depth);
