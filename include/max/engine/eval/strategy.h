/// \file strategy.h
#pragma once
#include "max/board/board.h"
#include "max/def.h"
#include "max/engine/score.h"

/// \ingroup eval
/// @{

/// \defgroup strat Strategic Evaluation
/// More complex analysis identifying strategies like placement on outpost squares
/// @{

/// Parameters for engine evaluation of strategic play
typedef struct {
    /// Bonus for knights and rooks placed on outpost squares
    max_score_t outpost;
} max_engine_strat_param_t;


/// Check if a pawn attacks the given square or may attack the given square after advancing.
/// \param [in] board The board to scan for attacking pawns
/// \param sq Square to check for attacks along the file of
/// \param enemy Color of the enemy's pieces
/// \return The number of pawns that may advance and attack the given square
max_score_t max_engine_pawn_attacks_span(max_board_t const *board, max_0x88_t sq, max_side_t enemy);

/// Get an outpost bonus for the given square.
/// \param outpost_bonus The score to return for an outpost square
/// \param [in] board The board to scan for enemy and friendly pawns
/// \param sq Square to check for outposting
/// \param side The side of the piece on the outpost square
/// \return The value of the outpost_bonus parameter
max_score_t max_engine_outpost(max_score_t outpost_bonus, max_board_t const *board, max_0x88_t sq, max_side_t side);

/// Get default values for all strategic evaluation scores.
/// \return Default strategic evaluation scores
MAX_INLINE_ALWAYS max_engine_strat_param_t max_engine_strat_param_default(void) {
    return (max_engine_strat_param_t){
        .outpost = 142
    };
}

/// @}

/// @}
