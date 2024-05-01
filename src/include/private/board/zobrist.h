#pragma once
#include "max/board/loc.h"
#include "max/board/piececode.h"
#include "max/board/zobrist.h"

/// \ingroup zobrist
/// @{

/// \name Private Functions
/// @{

/// Initialize all static zobrist element arrays using the provided RNG seed
/// \param [out] elems Zobrist elements table to initialize with random values
/// \param [in] seed The seed to use for the random number generator
void max_zobrist_elements_init(max_zobrist_elements_t *elems, uint64_t seed);

/// Get the zobrist hash element that identifies a piece on the given square
/// \param [in] elems Reference to statically initialized zobrist hash elements
/// \param [in] pos The 0x88 board position of the given piece
/// \param [in] piece The piece code of the given piece, only the type matters here
MAX_INLINE_ALWAYS
max_zobrist_t max_zobrist_position_element(max_zobrist_elements_t const *elems, max_0x88_t pos, max_piececode_t piece) {
    return elems->position[max_piececode_side(piece)][max_piececode_kind_index(piece)][max_0x88_to_6bit(pos).v];
}

/// Get the zobrist hash element identifying that the given side's castle rights have been removed
/// \param [in] elems Reference to statically initialized zobrist elements to lookup rom
/// \param [in] side The side that has lost castle rights
/// \param [in] kingside True if the given side lost kingside castle rights
MAX_INLINE_ALWAYS
max_zobrist_t max_zobrist_removed_castle_rights(max_zobrist_elements_t const *elems, max_side_t side, bool kingside) {
    return elems->castlerights[kingside | (side << 1)];
}

/// Get the zobrist hash element identifying that en passant is possible on the given file
/// \param [in] elems Reference to already initialized zobrist hash elements
/// \param [in] file File number that MUST be between 0 and 7 inclusive
MAX_INLINE_ALWAYS max_zobrist_t max_zobrist_ep_file(max_zobrist_elements_t const *elems, uint8_t file) {
    return elems->en_passant_file[file];
}

/// @}

/// @}
