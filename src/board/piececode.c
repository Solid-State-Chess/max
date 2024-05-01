#include "max/board/piececode.h"

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
