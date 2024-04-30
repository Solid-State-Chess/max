#pragma once
#include "max/board/loc.h"
#include "max/board/move.h"
#include "max/def.h"

MAX_INLINE_ALWAYS max_pmove_t max_pmove_new(max_0x88_t from, max_0x88_t to, max_movetag_t tag) {
    return (tag << MAX_PMOVE_TAG_POS) |
           (max_0x88_to_6bit(to) << MAX_PMOVE_TO_POS) |
            (max_0x88_to_6bit(from));
}

#ifdef MAX_TESTS
void max_move_unit_tests(void);
#endif
