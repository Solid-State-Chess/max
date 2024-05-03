#include "private/board/move.h"
#include "max/board/move.h"
#include "max/board/piececode.h"

static const uint8_t MAX_PIECECODE_FOR_PROMOTE_TAG[5] = {
    0,
    MAX_PIECECODE_KNIGHT,
    MAX_PIECECODE_BISHOP,
    MAX_PIECECODE_ROOK,
    MAX_PIECECODE_QUEEN,
};

#define MAX_MOVETAG_PROMOTE_MASK (0x07)

max_piececode_t max_piececode_for_movetag_promote(max_movetag_t tag, max_side_t side) {
    MAX_SANITY(max_movetag_is_promote(tag));
    return max_piececode_new(
        side,
        MAX_PIECECODE_FOR_PROMOTE_TAG[(tag & MAX_MOVETAG_PROMOTE_MASK)]
    );
}

#ifdef MAX_TESTS

void max_move_unit_tests(void) {
    
}

#endif
