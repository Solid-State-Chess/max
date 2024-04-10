#pragma once

#include "max/board.h"

#define MAX_ENGINE_MAX_MOVES (2048)

/// A signed score as the result of a board evaluation measured in centipawns
typedef int32_t max_score_t;

/// Diagnostic information for a best-move search
typedef struct {
    /// Number of nodes that have had the evaluation function executed
    uint64_t nodes;
    /// No. of nodes ignored due to futility pruning
    uint64_t futility_pruned;
} max_search_diagnostic_t;

/// State required at all stages of an alpha-beta search
typedef struct {
    max_move_t moves[MAX_ENGINE_MAX_MOVES];
} max_search_state_t;

/// Complete engine state including lookup tables, settings, and chessboard representation
typedef struct {
    max_board_t board;
    max_search_state_t search;
    /// Stack used by the board to reverse moves
    max_irreversible_t board_stack[16];
    max_search_diagnostic_t diagnostic;
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
/// returns true if a valid move is available to be played, and false otherwise
bool max_engine_search(max_engine_t *engine, max_searchresult_t *search, uint8_t depth);
