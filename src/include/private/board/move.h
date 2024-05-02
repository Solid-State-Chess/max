#pragma once
#include "max/assert.h"
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

/// Add the given move to the move list, ensuring that the capacity of the list is not exceeded if
/// debug assertions are enabled
MAX_INLINE_ALWAYS void max_movelist_add(max_movelist_t *list, max_smove_t move) {
    MAX_ASSERT(list->capacity > list->len);

#ifdef MAX_PACKED_MOVE
    list->buf[list->len] = max_pmove_create_from(move);
#else
    list->buf[list->len] = move;
#endif

    list->len += 1;
}

#ifdef MAX_TESTS
void max_move_unit_tests(void);
#endif


/// @}

/// @}
