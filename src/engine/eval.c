#include "max/def.h"
#include "max/engine.h"
#include "max/piece.h"
#include "private.h"

MAX_HOT MAX_INLINE_ALWAYS static
max_score_t max_evaluate_positions(max_piecelist_t *pieces, max_score_t const *pstbl, bool white) {
    static const max_bpos_t SQTRANSFORM[2] = {0x70, 0x00};

    max_score_t score = 0;

    for(max_lidx_t i = 0; i < pieces->len; ++i) {
        max_bpos_t pos = pieces->pos[i] ^ SQTRANSFORM[white];
        pos = (pos + (pos & 7)) >> 1;
        score += pstbl[pos];
    }

    return score;
}

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

    static max_score_t KNIGHT_PSTBL[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    };

    static max_score_t BISHOP_PSTBL[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    };

    static max_score_t ROOK_PSTBL[64] = {
          0,  0,  0,  0,  0,  0,  0,  0,
          5, 10, 10, 10, 10, 10, 10,  5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
          0,  0,  0,  5,  5,  0,  0,  0
    };

    static max_score_t QUEEN_PSTBL[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    static max_score_t KING_PSTBL[64] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    };
    
    max_score_t material = 
        (side->pawns.len   * MAX_PAWN_VALUE  ) +
        (side->knights.len * MAX_KNIGHT_VALUE) +
        (side->bishops.len * MAX_BISHOP_VALUE) +
        (side->rooks.len   * MAX_ROOK_VALUE  ) +
        (side->queens.len  * MAX_QUEEN_VALUE ) +
        (side->king.len    * MAX_KING_VALUE  );
    
    max_score_t position = 
        max_evaluate_positions((max_piecelist_t*)&side->pawns,   PAWN_PSTBL,   white) +
        max_evaluate_positions((max_piecelist_t*)&side->knights, KNIGHT_PSTBL, white) +
        max_evaluate_positions((max_piecelist_t*)&side->bishops, BISHOP_PSTBL, white) +
        max_evaluate_positions((max_piecelist_t*)&side->rooks,   ROOK_PSTBL,   white) +
        max_evaluate_positions((max_piecelist_t*)&side->queens,  QUEEN_PSTBL,  white) +
        max_evaluate_positions((max_piecelist_t*)&side->king,    KING_PSTBL,   white);
    

    return material + position;
}

max_score_t max_evaluate(max_engine_t *engine) {
    engine->diagnostic.nodes += 1;
    max_score_t white = max_evaluate_side(&engine->board.white, 0);
    max_score_t black = max_evaluate_side(&engine->board.black, 1);
    return white - black;
}
