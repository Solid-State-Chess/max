/// \file tt.h
#pragma once
#include <stdint.h>

/// \ingroup engine
/// @{


/// \defgroup tt Transposition Table
/// The all-important transposition table allowing the engine to perform iterative deepening search and massively
/// speeding up game tree searches by eliminating already-searched portions of the tree.
/// @{


#ifdef MAX_ZOBRIST_64
typedef uint32_t max_ttentry_key_t;
#else
typedef uint16_t max_ttentry_key_t;
#endif

/// An entry in the transposition table containing the result of a prior evaluation.
/// The size of this entry has massive effects on the memory footprint of the program as the transposition table
/// should be the largest object in the engine, so reducing it's size is extremely important.
/// It represents a best or refutation move in as compact a form as possible to reflect this, and packs all attributes into
/// as few bits as possible.
typedef struct {
    
} max_ttentry_t;

/// @}

/// @}
