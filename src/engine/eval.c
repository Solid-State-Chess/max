#include "max/board/board.h"
#include "max/board/op.h"
#include "max/def.h"
#include "max/engine/engine.h"
#include "max/board/piece.h"
#include "max/board/square.h"
#include "private.h"

MAX_HOT MAX_INLINE_ALWAYS static
max_score_t max_evaluate_positions(max_piecelist_t *pieces, max_score_t const *pstbl, bool white) {
    static const max_bpos_t SQTRANSFORM[2] = {0x70, 0x00};

    max_score_t score = 0;

    for(max_lidx_t i = 0; i < pieces->len; ++i) {
        max_bpos_t pos = pieces->pos[i] ^ SQTRANSFORM[white];
        pos = max_bpos_compress(pos);
        score += pstbl[pos];
    }

    return score;
}

/// Evaluate stacked pieces for the given side
MAX_HOT
MAX_INLINE_ALWAYS max_score_t max_evaluate_stacked_pawns(max_board_t *const board, max_pieces_t *pieces) {
    max_score_t penalty = 0;

    for(unsigned i = 0; i < pieces->pawns.len; ++i) {
        max_piececode_t piece = board->pieces[pieces->pawns.pos[i]];
        max_bpos_t above = max_bpos_inc(pieces->pawns.pos[i], max_board_friendly_pawn_advance_dir(board));
        //Don't need to check validity because pawns may not be on the final ranks
        if(max_bpos_valid(above)) {
            max_piececode_t above_piece = board->pieces[above];
            if(
                above_piece & (piece & MAX_PIECECODE_COLOR_MASK) &&
                (above_piece & MAX_PIECECODE_TYPE_MASK) == MAX_PIECECODE_PAWN
            ) {
                penalty -= 75;
            }
        }
    }

    return penalty;
}

#define MAX_CONTROLLED_SQUARE_VALUE (12)

MAX_HOT MAX_INLINE_ALWAYS
static max_score_t max_count_slider_squares(max_board_t *const board, max_bpos_t sq, max_increment_t const *const dirs) {
    if(max_board_is_pinned(board, sq)) {
        return -100;
    }

    max_score_t count = 0;
    for(unsigned i = 0; i < 4; ++i) {
        max_increment_t dir = dirs[i];
        max_bpos_t pos = sq;
        do {
            max_bpos_t pos = max_bpos_inc(pos, dir);
            count += MAX_CONTROLLED_SQUARE_VALUE;
        } while(max_bpos_valid(pos) && board->pieces[pos] == MAX_PIECECODE_EMPTY);
    }

    return count;
}

MAX_HOT MAX_INLINE_ALWAYS
static max_score_t max_evaluate_slider_control(max_board_t *const board, max_pieces_t *pieces) {
    max_score_t score = 0;

    for(unsigned i = 0; i < pieces->bishops.len; ++i) {
        max_bpos_t pos = pieces->bishops.pos[i];
        score += max_count_slider_squares(board, pos, MAX_DIAGONALS);
    }

    for(unsigned i = 0; i < pieces->rooks.len; ++i) {
        max_bpos_t pos = pieces->rooks.pos[i];
        score += max_count_slider_squares(board, pos, MAX_CARDINALS);
    }

    for(unsigned i = 0; i < pieces->queens.len; ++i) {
        max_bpos_t pos = pieces->queens.pos[i];
        score += max_count_slider_squares(board, pos, MAX_DIAGONALS);
        score += max_count_slider_squares(board, pos, MAX_CARDINALS);
    }

    return score;
}

MAX_HOT
MAX_INLINE_ALWAYS
static max_score_t max_evaluate_connected_rooks(max_board_t *const board, max_pieces_t *pieces) {
    if(pieces->rooks.len == 2) {
        max_bpos_t pos   = pieces->rooks.pos[0];
        max_bpos_t rook2 = pieces->rooks.pos[1];

        max_increment_t dir = MAX_DIRECTION_BY_DIFF[max_bpos_diff(pos, rook2)];
        if(dir == 0) {
            return -10;
        }

        do {
            pos = max_bpos_inc(pos, dir);
        } while(board->pieces[pos] == MAX_PIECECODE_EMPTY);

        if(pos == rook2) {
            return 50;
        } else {
            return -10;
        }
    }

    return 0;
}

MAX_HOT MAX_INLINE_ALWAYS
static max_score_t max_evaluate_bishop_pair(max_board_t *const board, max_pieces_t *side) {
    if(side->bishops.len >= 2) {
        return 125;
    } else {
        return 0;
    }
}

/// Evaluate the material for one side of a chessboard
MAX_HOT
MAX_INLINE_ALWAYS
static max_score_t max_evaluate_side(max_board_t *board, max_pieces_t *side, unsigned white) {
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

    max_score_t extra[2] = {0, 0};
    
    max_score_t material = 
        (side->pawns.len   * (MAX_PAWN_VALUE   + extra[white])) +
        (side->knights.len * (MAX_KNIGHT_VALUE + extra[white])) +
        (side->bishops.len * (MAX_BISHOP_VALUE + extra[white])) +
        (side->rooks.len   * (MAX_ROOK_VALUE   + extra[white])) +
        (side->queens.len  * (MAX_QUEEN_VALUE  + extra[white])) +
        (side->king.len    * (MAX_KING_VALUE   + extra[white]));
    
    max_score_t position = 
        max_evaluate_positions((max_piecelist_t*)&side->pawns,   PAWN_PSTBL,   white) +
        max_evaluate_positions((max_piecelist_t*)&side->knights, KNIGHT_PSTBL, white) +
        max_evaluate_positions((max_piecelist_t*)&side->bishops, BISHOP_PSTBL, white) +
        max_evaluate_positions((max_piecelist_t*)&side->rooks,   ROOK_PSTBL,   white) +
        max_evaluate_positions((max_piecelist_t*)&side->queens,  QUEEN_PSTBL,  white) +
        max_evaluate_positions((max_piecelist_t*)&side->king,    KING_PSTBL,   white);
    
    return material +
        position +
        max_evaluate_stacked_pawns(board, side) +
        //max_evaluate_slider_control(board, side) +
        max_evaluate_bishop_pair(board, side);
}

max_score_t max_evaluate(max_engine_t *engine) {
    engine->diagnostic.nodes += 1;
    max_score_t white = max_evaluate_side(&engine->board, &engine->board.white, 0);
    max_score_t black = max_evaluate_side(&engine->board, &engine->board.black, 1);
    return (white - black);
}
