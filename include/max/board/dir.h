/// \file dir.h

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "max/board/loc.h"
#include "max/def.h"

/// \ingroup loc
/// @{

/// \defgroup x88dir x88 Coordinate Directions
///
/// @{

/// Wrapper struct for a signed increment applied to a #max_0x88_t board location.
/// Increments are used extensively in move generation to determine the next location along a ray,
/// or to store the offsets that a knight jumps to.
typedef struct { int8_t v; } max_0x88_dir_t;


/// Shift the given 0x88 board position as by the given amount.
/// This shift may result in an invalid board position, which must be checked by the user.
/// \param loc [in] The location to shift by the amount specified in dir
/// \param dir [in] A signed increment specifying how to shift the given position
/// \return A shifted board location, which may be invalid if the shift moved it off the board
MAX_INLINE_ALWAYS max_0x88_t max_0x88_move(max_0x88_t loc, max_0x88_dir_t dir) {
    loc.v += dir.v;
    return loc;
}

/// Get the single-square direction increment (if any) that moves from the given square to the given square.
/// \param [in] from The board position that a ray is cast from
/// \param [in] to The board position that a ray reaches
/// \param [out] dir The direction increment that will be filled if there is a straight line connecting the two squares
/// \return true if there was a straight line connecting the two squares
bool max_0x88_line(max_0x88_t from, max_0x88_t to, max_0x88_dir_t *dir);

/// @}

/// @}
