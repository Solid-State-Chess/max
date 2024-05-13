#include "max/board/piececode.h"
#include "private/board/piececode.h"

max_pieceindex_t max_piececode_kind_index(max_piececode_t piece) {
    static uint8_t INDICES[16] = {
        [MAX_PIECECODE_PAWN]   = MAX_PIECEINDEX_PAWN,
        [MAX_PIECECODE_KNIGHT] = MAX_PIECEINDEX_KNIGHT,
        [MAX_PIECECODE_BISHOP] = MAX_PIECEINDEX_BISHOP,
        [MAX_PIECECODE_ROOK]   = MAX_PIECEINDEX_ROOK,
        [MAX_PIECECODE_QUEEN]  = MAX_PIECEINDEX_QUEEN,
        [MAX_PIECECODE_KING]   = MAX_PIECEINDEX_KING,
    };

    return INDICES[piece.v & MAX_PIECECODE_TYPE_MASK];
}

#ifdef MAX_TESTS
#include "private/test.h"

void max_piececode_unit_tests(void) {
    max_piececode_t pawn = max_piececode_new(MAX_PIECECODE_WHITE, MAX_PIECECODE_PAWN);

    max_piececode_t nonsliders[] = {
        max_piececode_new(MAX_SIDE_WHITE, MAX_PIECECODE_PAWN),
        max_piececode_new(MAX_SIDE_WHITE, MAX_PIECECODE_KNIGHT),
        max_piececode_new(MAX_SIDE_WHITE, MAX_PIECECODE_KING)
    };

    const unsigned nonsliders_len = sizeof(nonsliders) / sizeof(nonsliders[0]);

    for(unsigned i = 0; i < nonsliders_len; ++i) {
        ASSERT(!max_piececode_match(nonsliders[i], MAX_PIECEMASK_DIAGONAL), "Jumping piece should not match piece mask for diagonal attackers");
        ASSERT(!max_piececode_match(nonsliders[i], MAX_PIECEMASK_CARDINAL), "Jumping piece should not match piece mask for cardinal attackers");
    }
}
#endif
