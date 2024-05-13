#pragma once
#include "max/board/board.h"

/// \defgroup engine Chess Engine
/// The actual chess engine including search algorithm and transposition table.
/// @{

/// All state required by the chess engine to search a game tree and evaluate positions.
/// Includes a pointer to a #max_board_t that will be used to search a tree by making and unmaking moves,
/// and configurable evaluation parameters.
typedef struct {
    max_board_t *board;
} max_engine_t;

/// @}
