/// \file tt.h
#pragma once
#include "max/engine/score.h"
#include <stdint.h>

/// \ingroup engine
/// @{


/// \defgroup tt Transposition Table
/// The all-important transposition table allowing the engine to perform iterative deepening search and massively
/// speeding up game tree searches by eliminating already-searched portions of the tree.
/// @{

/// Key portion of a zobrist hash, created after masking away the lower index bits.
/// This is half the size of a true zobrist hash to aggresively reduce the size of the transposition table,
/// at the cost of increased table collision rate if the key portion is actually larger than the storage type.
/// @{

#ifdef MAX_ZOBRIST_64
typedef uint32_t max_ttentry_key_t;
#else
typedef uint16_t max_ttentry_key_t;
#endif

/// @}

/// An entry in the transposition table containing the result of a prior evaluation.
/// The size of this entry has massive effects on the memory footprint of the program as the transposition table
/// should be the largest object in the engine, so reducing it's size is extremely important.
/// It represents a best or refutation move in as compact a form as possible to reflect this, and packs all attributes into
/// as few bits as possible.
typedef struct {
    /// Key portin of the zobrist hash used to access this position, used to ensure that all index
    /// collisions do not also result in a full hash collision, which would case the engine to use
    /// invalid transposition table entries far more often.
    max_ttentry_key_t key;
    /// Score of the node as determined by the search when this entry was created.
    /// If the node was alpha or beta cutoff, this is the value that caused the cutoff.
    max_score_t score;
} max_ttentry_t;

/// A transposition table of configurable power-of-two capacity.
typedef struct {
    max_ttentry_t *buf;
    uint8_t nbit; 
} max_ttbl_t;

/// @}

/// @}
