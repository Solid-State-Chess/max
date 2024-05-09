/// \file dir.h

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "max/assert.h"
#include "max/board/loc.h"
#include "max/board/piececode.h"
#include "max/def.h"

/// \ingroup loc
/// @{

/// \defgroup x88dir x88 Coordinate Directions
/// Signed increments that can be applied to a 0x88 board location.
/// @{

/// Wrapper type for a signed increment applied to a #max_0x88_t board location.
/// Increments are used extensively in move generation to determine the next location along a ray,
/// or to store the offsets that a knight jumps to.
typedef int8_t max_0x88_dir_t;


enum {
    /// Invalid direction that does not increment a position at all
    MAX_0x88_DIR_INVALID = 0,

    MAX_0x88_DIR_UP    =  0x10,
    MAX_0x88_DIR_DOWN  = -0x10,
    MAX_0x88_DIR_RIGHT =  0x01,
    MAX_0x88_DIR_LEFT  = -0x01,

    MAX_0x88_DIR_UR    = MAX_0x88_DIR_UP + MAX_0x88_DIR_RIGHT,
    MAX_0x88_DIR_UL    = MAX_0x88_DIR_UP + MAX_0x88_DIR_LEFT,
    MAX_0x88_DIR_DR    = MAX_0x88_DIR_DOWN + MAX_0x88_DIR_RIGHT,
    MAX_0x88_DIR_DL    = MAX_0x88_DIR_DOWN + MAX_0x88_DIR_LEFT,
};

/// Length of the #MAX_0x88_DIAGONALS array
#define MAX_0x88_DIAGONALS_LEN (4)

/// Array of all four diagonal directions that a bishop would slide across.
/// Internally, this is a pointer to a segment of #MAX_0x88_RAYS.
extern max_0x88_dir_t *MAX_0x88_DIAGONALS;

/// Length of the #MAX_0x88_CARDINALS array
#define MAX_0x88_CARDINALS_LEN (4)

/// Array of all four cardinal directions that a rook would slide across.
/// Internally, this is a pointer to a segment of #MAX_0x88_RAYS.
extern max_0x88_dir_t *MAX_0x88_CARDINALS;

/// Length of the #MAX_0x88_RAYS array
#define MAX_0x88_RAYS_LEN (8)

/// Array of all eight cardinal and diagonal ray directions,
/// for how a queen would move
extern max_0x88_dir_t MAX_0x88_RAYS[MAX_0x88_RAYS_LEN];


/// Shift the given 0x88 board position as by the given amount.
/// This shift may result in an invalid board position, which must be checked by the user.
/// \param loc [in] The location to shift by the amount specified in dir
/// \param dir [in] A signed increment specifying how to shift the given position
/// \return A shifted board location, which may be invalid if the shift moved it off the board
MAX_INLINE_ALWAYS max_0x88_t max_0x88_move(max_0x88_t loc, max_0x88_dir_t dir) {
    loc.v += dir;
    return loc;
}

/// Get the single-square direction increment (if any) that moves from the given square to the given square.
/// \param [in] from The board position that a ray is cast from
/// \param [in] to The board position that a ray reaches
/// \return  The direction increment of a straight line connecting the two squares, or MAX_0x88_DIR_INVALID if there is no line between the squares
max_0x88_dir_t max_0x88_line(max_0x88_t from, max_0x88_t to);

/// Get a piecemask for a piece that attacks the given direction.
/// \param dir Ray direction along which a piece moves
/// \return A piece mask that matches pieces that can move along the given ray direction, or an empty piecemask if dir is MAX_0x88_DIR_INVALID
MAX_INLINE_ALWAYS max_piecemask_t max_0x88_piecemask_for_dir(max_0x88_dir_t dir) {
    switch(dir) {
        case MAX_0x88_DIR_UR:
        case MAX_0x88_DIR_UL:
        case MAX_0x88_DIR_DR:
        case MAX_0x88_DIR_DL:
            return MAX_PIECEMASK_DIAGONAL;
        break;

        case MAX_0x88_DIR_INVALID: return MAX_PIECEMASK_EMPTY; break;
        default:
            MAX_SANITY(
                dir == MAX_0x88_DIR_UP ||
                dir == MAX_0x88_DIR_DOWN ||
                dir == MAX_0x88_DIR_RIGHT ||
                dir == MAX_0x88_DIR_LEFT
            );
            return MAX_PIECEMASK_CARDINAL;
        break;
    }
}


/// @}

/// @}
