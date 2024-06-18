#include "private/board/move.h"
#include "max/board/move.h"
#include "max/board/piececode.h"

static const uint8_t MAX_PIECECODE_FOR_PROMOTE_TAG[6] = {
    [MAX_MOVETAG_PKNIGHT] = MAX_PIECECODE_KNIGHT,
    [MAX_MOVETAG_PBISHOP] = MAX_PIECECODE_BISHOP,
    [MAX_MOVETAG_PROOK] = MAX_PIECECODE_ROOK,
    [MAX_MOVETAG_PQUEEN] = MAX_PIECECODE_QUEEN,
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
