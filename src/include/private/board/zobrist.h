#pragma once
#include "max/board/zobrist.h"

/// \ingroup zobrist
/// @{

/// \name Private Functions
/// @{

/// Initialize all static zobrist element arrays using the provided RNG seed
/// \param [out] elems Zobrist elements table to initialize with random values
/// \param [in] seed The seed to use for the random number generator
void max_zobrist_elements_init(max_zobrist_elements_t *elems, max_zobrist_t seed);

/// Get the zobrist hash element that identifies a piece on the given square
/// \param [in] elems Reference to statically initialized zobrist hash elements
/// \param [in] pos The 0x88 board position of the given piece
/// \param [in] piece The piece code of the given piece, only the type matters here
max_zobrist_t max_zobrist_position_element(max_zobrist_elements_t const *elems, max_0x88_t pos, max_piececode_t piece);

/// @}

/// @}
