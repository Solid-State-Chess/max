#pragma once
#include "max/assert.h"
#include "max/board/move.h"
#include "max/def.h"

/// \ingroup move
/// @{

/// \name Private Functions
/// @{

/// Add the given move to the move list, ensuring that the capacity of the list is not exceeded if
/// debug assertions are enabled
MAX_INLINE_ALWAYS void max_movelist_add(max_movelist_t *list, max_smove_t move) {
    MAX_ASSERT(list->capacity > list->len && "Move list capacity exceeded");
    list->buf[list->len] = move;
    list->len += 1;
}

#ifdef MAX_TESTS
void max_move_unit_tests(void);
#endif


/// @}

/// @}
