#pragma once
#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/def.h"

/// \ingroup move
/// @{

/// \name Private Functions
/// @{

/// Pack a move structure from the given sparse move structure
/// \param [in] move The move to pack into 2 bytes
/// \return A packed move with the same attributes as #move
MAX_INLINE_ALWAYS max_pmove_t max_pmove_create_from(max_smove_t move) {
    return (move.tag << MAX_PMOVE_TAG_POS) |
           (max_0x88_to_6bit(move.to).v << MAX_PMOVE_TO_POS) |
           (max_0x88_to_6bit(move.from).v);
}

#ifdef MAX_TESTS
void max_move_unit_tests(void);
#endif


/// @}

/// @}
