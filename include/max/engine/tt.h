#pragma once
/// \file
/// Transposition table definitions

/// A single entry in a transposition table containing the depth of analysis,
/// the score and bounds of the search, and the full zobrist key of the position
typedef struct {
    
} max_ttentry_t;

/// Transposition table storing the results of previous analysis, used to accelerate searching
/// massively and to enable iterative deepening
typedef struct {
     
} max_ttbl_t;
