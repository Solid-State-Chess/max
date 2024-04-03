#include "max/def.h"
#include "max/engine.h"
#include "max/piece.h"
#include "private.h"

#define MAX_PAWN_VALUE    100
#define MAX_KNIGHT_VALUE  300
#define MAX_BISHOP_VALUE  310
#define MAX_ROOK_VALUE    500
#define MAX_QUEEN_VALUE   900
#define MAX_KING_VALUE    100000


/// Evaluate the material for one side of a chessboard
MAX_HOT
MAX_INLINE_ALWAYS
static max_score_t max_evaluate_side(max_pieces_t *side, unsigned white) {
    static max_score_t PAWN_PSTBL[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    };
    
    max_score_t material = 
        (side->pawns.len   * MAX_PAWN_VALUE) +
        (side->knights.len * MAX_KNIGHT_VALUE) +
        (side->bishops.len * MAX_BISHOP_VALUE) +
        (side->rooks.len   * MAX_ROOK_VALUE)   +
        (side->queens.len  * MAX_QUEEN_VALUE) +
        (side->king.len    * MAX_KING_VALUE);
    
    for(max_lidx_t i = 0; i < side->pawns.len; ++i) {
        max_bpos_t pos;
        if(white) {
            pos = max_bpos_new(side->pawns.pos[i] & 0x7, (7 - (side->pawns.pos[i] >> 4)));
        } else {
            pos = side->pawns.pos[i];
        }

        pos = (pos + (pos & 7)) >> 1;

        material += PAWN_PSTBL[pos];
    }

    return material;
}

max_score_t max_evaluate(max_engine_t *engine) {
    max_score_t white = max_evaluate_side(&engine->board.white, 1);
    max_score_t black = max_evaluate_side(&engine->board.black, 0);
    return white - black;
}
