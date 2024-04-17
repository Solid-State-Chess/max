#pragma once
/// \file
/// Transposition table definitions allowing the chess engine to cache the results of a previous search, massively reducing the search
/// space and allowing for iterative deepening.

#include "max/engine/score.h"
#include <stdbool.h>
#include <stdint.h>

/// \defgroup transposition Transposition Table
/// Definitions for types that utilize Zobrist hash keys to index a cache that allows the
/// engine to 'remember' the results of previous searches. The size of both the cache and
/// the zobrist hash are essential to reducing hash collisions and index collisions.
/// @{

/// Part of a zobrist hash that isn't used to index the transposition table array in a 
/// #max_ttbl_t.
typedef uint16_t max_zobrist_key_t;

/// Type of evaluation that was performed to store a score inside a transposition table.
/// \see MAX_TTENTRY_TYPE_PV
/// \see MAX_TTENTRY_TYPE_LOWER
/// \see MAX_TTENTRY_TYPE_UPPER
typedef uint8_t max_ttentry_type_t;

enum {
    /// Node that was analyzed which raised the lower bound of the score.
    /// All moves at this node were searched and the score given is exact
    MAX_TTENTRY_TYPE_PV    = 0,
    /// Node that was analyzed which 'failed high'.
    /// The score stored in a transposition table entry of this type is a lower bound on the true score of the node
    MAX_TTENTRY_TYPE_LOWER = 1,
    /// Node that was analyzed which 'failed low'.
    /// All moves were searched, and each move failed to raise the lower bound alpha.
    /// The score stored in a transposition table entry of this type is an upper bound on the score.
    MAX_TTENTRY_TYPE_UPPER = 2,
};

/// Packed information for a transposition table entry including the node type and depth of analysis.
/// Note that this struct could reasonably be packed into a single byte, but for now it is not
/// to preserve aligned field access in #max_ttentry_t.
typedef struct {
    /// Depth that the node was searched to.
    uint8_t depth : 7;
    /// Gravestone marker that is true for all transposition table entries that do not yet have a value.
    bool gravestone : 1;
    /// The type of node that this is, changes what the score actually means.
    /// \see #max_ttentry_type_t
    max_ttentry_type_t node_type;
} max_ttentry_attr_t;

#pragma pack(push)

/// A single entry in a transposition table containing the depth of analysis,
/// the score and bounds of the search, and the full zobrist key of the position
typedef struct {
    /// Part of the zobrist hash that was not masked to form the index of this element in the array.
    /// \see max_ttbl_t::array
    max_zobrist_key_t key_part;
    /// The score that was determined after searching this node to a depth given in 
    /// #attr. Note that the score may not be exact - instead it may be an upper or lower bound
    /// on the true score of the node.
    max_score_t score;
    /// Attributes about the cached analysis including the depth and node type.
    /// This gives required information about the #score that must be considered when using this entry
    /// in a search.
    max_ttentry_attr_t attr;
} max_ttentry_t;

#pragma pack(pop)

/// Number of bits used to index the #max_ttbl_t::array field.
/// Also determines the size of the array (2^#MAX_TTBL_INDEX_BITS).
#define MAX_TTBL_INDEX_BITS (17)

/// Power-of-two length of the #max_ttbl_t::array field.
/// Note that for now the length is fixed in a define, user-provided buffers must be supported
/// at a later time.
/// \see MAX_TTBL_INDEX_BITS
#define MAX_TTBL_LEN (1 << MAX_TTBL_INDEX_BITS)

/// Transposition table storing the results of previous analysis, used to accelerate searching
/// massively and to enable iterative deepening
typedef struct {
    max_ttentry_t array[MAX_TTBL_LEN];
} max_ttbl_t;


/// Set all entries in the given transposition table to a gravestone value indicating that
/// there is no entry at the given hash.
void max_ttbl_new(max_ttbl_t *array);

//@}
