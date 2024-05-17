/// \file score.h
#pragma once
#include "max/board/move.h"
#include "max/def.h"
#include <stdint.h>


/// \ingroup engine
/// @{


/// A signed score in the range [-20000, 20000] indicating the value of a given object.
/// Used by the engine to score the full value of positions when searching a game tree.
/// \see #max_smallscore_t
typedef int16_t max_score_t;

/// The highest score possible to store in a #max_score_t, used to create a null window
/// for alpha-beta search.
#define MAX_SCORE_HIGHEST (INT16_MAX - 1000)
/// The lowest score possible to store in a #max_score_t, used to create a null window for
/// alpha-beta search.
#define MAX_SCORE_LOWEST  (INT16_MIN + 1000)

/// A signed score in centipawns in the range [-128, 127].
/// This is a compressed version of #max_score_t used to store piece-square tables and other lookup
/// tables whose elements all value less than a pawn as compact as possible.
/// \see #max_score_t
typedef int8_t max_smallscore_t;

/// \name Node Score Types
/// During alpha-beta search, some nodes will not be evaluated as deeply as other nodes
/// because their scores meet certain criteria.
/// @{

enum {
    /// Principle Variation Node.
    /// All child nodes were searched, and the score returned successfully raised the lower bound of the score,
    /// so the score associated with this node is exact.
    MAX_NODEKIND_PV = 0,
    /// Cut Node.
    /// This node failed high when one of the child nodes exceeded the upper bound on the score.
    /// Not all child nodes were searched for this node, so the score returned is a lower bound on the true score of the
    /// node.
    MAX_NODEKIND_CUT = 1,
    /// All Node.
    /// All children of this node were searched, and each failed to exceed the lower bound on the score.
    /// The score associated with this node is an upper bound on the true value of the node.
    MAX_NODEKIND_ALL = 2,
};

/// A 2-bit value specifying the kind of node a given scored position was.
typedef uint8_t max_nodekind_t;

/// @}

/// A list of scores associated with every move in a #max_movelist_t.
/// This is used by the search algorithm in order to store the results of iterative deepening between calls to
/// #max_engine_search.
typedef struct {
    /// The move that raised alpha or caused a beta cutoff.
    max_smove_t bestmove;
    /// Score determined for the given move the last time it was searched.
    max_score_t score; 
    /// How this node was actually evaluated - determines what the associated score means in context.
    max_nodekind_t kind;
    /// Depth that the move was scored up to.
    uint8_t depth;
} max_nodescore_t;

/// Maximum number of moves to analyze per game ply - this limit is used to
/// stop recursion before we overflow the move list, and to limit the capacity of score lists for each ply.
#define MAX_ENGINE_MAX_MOVES_PER_PLY (80)

/// A list containing both moves and associated scores, used to sort move lists during alpha-beta search and in between iterations
/// of iterative deepening to ensure that the most promising nodes are re-evaluated first
typedef struct {
    /// A list of scores associated with the moves stored in the movelist, the length of this static array is determined by the
    /// length of the moves list.
    max_score_t scores[MAX_ENGINE_MAX_MOVES_PER_PLY];
    /// The move list containing moves that are associated with scores at the same index in the #
    max_movelist_t moves;
} max_scorelist_t;

/// Reset the given scores list, setting its moves to the given movelist.
/// Note that scores will be left undefined and must be reassigned with max_scorelist_score
MAX_INLINE_ALWAYS void max_scorelist_reset(max_scorelist_t *scorelist, max_movelist_t moves) {
    scorelist->moves = moves;
}

/// Assign as score to the move located at the given index in the movelist.
MAX_INLINE_ALWAYS void max_scorelist_score(max_scorelist_t *scorelist, uint8_t idx, max_score_t score) {
    scorelist->scores[idx] = score;
}

/// Sort the given list by the contained scores, also sorting the contained move list in place.
/// Sorting the move list before evaluation improves alpha beta search by encouraging beta cutoffs.
void max_scorelist_sort(max_scorelist_t *scorelist);

/// @}
