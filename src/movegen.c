#include "max/board.h"
#include "max/def.h"
#include "max/move.h"
#include "max/piece.h"
#include "max/square.h"
#include "private.h"
#include <stdbool.h>

const max_increment_t KNIGHT_MOVES[8] = {
    MAX_INCREMENT_UP    + MAX_INCREMENT_UR,
    MAX_INCREMENT_UP    + MAX_INCREMENT_UL,
    MAX_INCREMENT_DOWN  + MAX_INCREMENT_DR,
    MAX_INCREMENT_DOWN  + MAX_INCREMENT_DL,
    MAX_INCREMENT_RIGHT + MAX_INCREMENT_UR,
    MAX_INCREMENT_RIGHT + MAX_INCREMENT_DR,
    MAX_INCREMENT_LEFT  + MAX_INCREMENT_UL,
    MAX_INCREMENT_LEFT  + MAX_INCREMENT_DL,
};

/// Generate all quiet and loud sliding moves in the given direction
static MAX_INLINE_ALWAYS void max_slidegen_all(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bpos_t start,
    max_increment_t dir
) {
    max_bpos_t pos = start;
    for(;;) {
        pos = max_bpos_inc(pos, dir);
        if(!max_bpos_valid(pos)) { break; }
        max_piececode_t square = board->pieces[pos];
        if(square == MAX_PIECECODE_EMPTY) {
            max_movelist_add(moves, max_move_normal(start, pos));
        } else {
            if(square & enemy) {
                max_movelist_add(moves, max_move_capture(start, pos));
            }
            break;
        }
    }
}

/// Generate only attacks in the given sliding direction
static MAX_INLINE_ALWAYS void max_slidegen_loud(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bpos_t start,
    max_increment_t dir
) {
    max_bpos_t pos = start;
    max_piececode_t square;
    for(;;) {
        pos = max_bpos_inc(pos, dir);
        if(!max_bpos_valid(pos)) { return; }
        square = board->pieces[pos];
        if(square != MAX_PIECECODE_EMPTY) { break; }
    }

    if(square & enemy) {
        max_movelist_add(moves, max_move_capture(start, pos));
    }
}

#define BISHOPSLIDE(fn) do {                          \
    fn(moves, board, enemy, start, MAX_INCREMENT_UR); \
    fn(moves, board, enemy, start, MAX_INCREMENT_DR); \
    fn(moves, board, enemy, start, MAX_INCREMENT_UL); \
    fn(moves, board, enemy, start, MAX_INCREMENT_DL); \
} while(0)

static MAX_INLINE_ALWAYS void max_bishopgen_all(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bpos_t start
) {
    BISHOPSLIDE(max_slidegen_all); 
}
static MAX_INLINE_ALWAYS void max_bishopgen_loud(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bpos_t start
) {
    BISHOPSLIDE(max_slidegen_loud); 
}

#undef BISHOPSLIDE

#define ROOKSLIDE(fn) do {                               \
    fn(moves, board, enemy, start, MAX_INCREMENT_UP);    \
    fn(moves, board, enemy, start, MAX_INCREMENT_RIGHT); \
    fn(moves, board, enemy, start, MAX_INCREMENT_LEFT);  \
    fn(moves, board, enemy, start, MAX_INCREMENT_DOWN);  \
} while(0)

/// Generate all valid moves for the given rook
static MAX_INLINE_ALWAYS void max_rookgen_all(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bpos_t start
) {
    ROOKSLIDE(max_slidegen_all); 
}

/// Generate all captures for the given rook
static MAX_INLINE_ALWAYS void max_rookgen_loud(
    max_movelist_t *const moves,
    max_board_t *const board,
    max_piececode_t enemy,
    max_bpos_t start
) {
    ROOKSLIDE(max_slidegen_loud); 
}

#undef ROOKSLIDE


//Rank masks for promotions
static max_bpos_t PAWN_PROMOTERANK[2] = {MAX_RANK_8, MAX_RANK_1};

MAX_HOT
inline static void max_board_pawnmovegen_quiet(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_piececode_t enemy,
    uint8_t side,
    max_bpos_t pos
) {
    //Homerow lookup table, indexed by the current side to move
    static max_bpos_t PAWN_HOMEROW[2]  = {MAX_RANK_2, MAX_RANK_7};

    max_bpos_t up = max_bpos_inc(pos, PAWN_INC[side]);

    if(max_bpos_valid(up)) {
        if((up & MAX_RANK_MASK) != PAWN_PROMOTERANK[side] && board->pieces[up] == MAX_PIECECODE_EMPTY) {
            max_movelist_add(moves, max_move_normal(pos, up));
            max_bpos_t up2 = max_bpos_inc(up, PAWN_INC[side]);
            if((pos & MAX_RANK_MASK) == PAWN_HOMEROW[side] && board->pieces[up2] == MAX_PIECECODE_EMPTY) {
                max_movelist_add(moves, max_move_new(pos, up2, MAX_MOVE_DOUBLE));
            }
        }
    }
}

MAX_HOT MAX_INLINE_ALWAYS static
bool max_board_pawn_promotegen(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_bpos_t pos,
    max_bpos_t square,
    uint8_t side,
    max_move_attr_t flags
) {
    if(max_bpos_valid(square) && (square & MAX_RANK_MASK) == PAWN_PROMOTERANK[side]) {
        /*max_movelist_add(moves, max_move_new(pos, square, flags | MAX_MOVE_PROMOTE_KNIGHT));
        max_movelist_add(moves, max_move_new(pos, square, flags | MAX_MOVE_PROMOTE_BISHOP));
        max_movelist_add(moves, max_move_new(pos, square, flags | MAX_MOVE_PROMOTE_ROOK));
        max_movelist_add(moves, max_move_new(pos, square, flags | MAX_MOVE_PROMOTE_QUEEN));*/
        return true;
    }

    return false;
}

MAX_HOT
inline static void max_board_pawnmovegen_loud(
    max_board_t *const board,
    max_movelist_t *const moves,
    max_piececode_t enemy_color,
    uint8_t side,
    max_bpos_t pos
) {
    //Rank that an enemy pawn would be en passanted at
    static max_bpos_t PAWN_EPRANK[2] = {MAX_RANK_5, MAX_RANK_4};

    max_bpos_t up = max_bpos_inc(pos, PAWN_INC[side]);
    if(max_bpos_valid(up) && board->pieces[up] == MAX_PIECECODE_EMPTY) {
        max_board_pawn_promotegen(board, moves, pos, up, side, 0);
    }
    
    max_bpos_t right = max_bpos_inc(up, MAX_INCREMENT_RIGHT);
    if(max_bpos_valid(right) && (board->pieces[right] & enemy_color)) {
        if(!max_board_pawn_promotegen(board, moves, pos, right, side, MAX_MOVE_CAPTURE)) {
            max_movelist_add(moves, max_move_capture(pos, right));
        }
    }

    max_bpos_t left = max_bpos_inc(up, MAX_INCREMENT_LEFT);
    if(max_bpos_valid(left) && (board->pieces[left] & enemy_color)) {
        if(!max_board_pawn_promotegen(board, moves, pos, left, side, MAX_MOVE_CAPTURE)) {
            max_movelist_add(moves, max_move_capture(pos, left));
        }
    }
    

    //If the EP file is invalid on the stack, then this will be an invalid index
    max_bpos_t epsquare = (board->stack[board->ply] & MAX_PLYPLATE_EP_MASK) | PAWN_EPRANK[side];

    if(max_bpos_valid(epsquare)) {
        if(max_bpos_inc(pos, MAX_INCREMENT_RIGHT) == epsquare || max_bpos_inc(pos, MAX_INCREMENT_LEFT) == epsquare) {
            max_bpos_t moveto = max_bpos_inc(epsquare, PAWN_INC[side]);
            max_movelist_add(moves, max_move_new(pos, moveto, MAX_MOVE_EN_PASSANT));
        }
    }
}

MAX_HOT
void max_board_movegen_pseudo(max_board_t *const board, max_movelist_t *const moves) {
    uint8_t side = board->ply & 1;
    max_pieces_t *state = &board->sides[side];
    max_pieces_t *enemy = &board->sides[side ^ 1];

    max_piececode_t enemy_color = MAX_PIECECODE_BLACK >> side;

    for(max_lidx_t i = 0; i < state->pawns.len; ++i) {
        max_bpos_t pos = state->pawns.pos[i];
        max_board_pawnmovegen_quiet(board, moves, enemy_color, side, pos);
        max_board_pawnmovegen_loud(board, moves, enemy_color, side, pos); 
    }

    #define NORMALMOVE(from, to) do {                                       \
        max_bpos_t dest = (to);                                             \
        if(max_bpos_valid(dest)) {                                          \
            max_piececode_t piece = board->pieces[dest];                    \
            if(piece == MAX_PIECECODE_EMPTY) {                              \
                max_movelist_add(moves, max_move_normal((from), dest));     \
            } else if(piece & enemy_color) {                                \
                max_movelist_add(moves, max_move_capture((from), dest));    \
            }                                                               \
        }                                                                   \
    } while(0)

    for(max_lidx_t i = 0; i < state->knights.len; ++i) {
        max_bpos_t pos = state->knights.pos[i];
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UL));

        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DL));

        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_DR));


        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_UL));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_DL));
    }

    for(max_lidx_t i = 0; i < state->bishops.len; ++i) {
        max_bishopgen_all(moves, board, enemy_color, state->bishops.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->rooks.len; ++i) {
        max_rookgen_all(moves, board, enemy_color, state->rooks.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->queens.len; ++i) {
        max_rookgen_all(moves, board, enemy_color, state->queens.pos[i]);
        max_bishopgen_all(moves, board, enemy_color, state->queens.pos[i]);
    }
    
    if(state->king.len != 0) {
        max_bpos_t kingpos = state->king.pos[0];
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UP));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DOWN));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_LEFT));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UR));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UL));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DR));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DL));
        
        max_plyplate_t plate = board->stack[board->ply];
        if(plate & max_kcastle_flag(board->ply)) {
            max_piececode_t color = MAX_PIECECODE_WHITE << side;

            max_bpos_t between[3] = {
                max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT),
                max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT * 2),
                max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT * 3),
            };
                    if(board->pieces[between[0]] == MAX_PIECECODE_EMPTY && board->pieces[between[1]] == MAX_PIECECODE_EMPTY && board->pieces[between[2]] == (MAX_PIECECODE_ROOK | color)) {
                max_movelist_add(moves, max_move_new(kingpos, between[1], MAX_MOVE_KCASTLE));
            }
        }
 
        if(plate & max_qcastle_flag(board->ply)) {
            max_piececode_t color = MAX_PIECECODE_WHITE << side;
            max_bpos_t between[4] = {
                max_bpos_inc(kingpos, MAX_INCREMENT_LEFT),
                max_bpos_inc(kingpos, MAX_INCREMENT_LEFT * 2),
                max_bpos_inc(kingpos, MAX_INCREMENT_LEFT * 3),
                max_bpos_inc(kingpos, MAX_INCREMENT_LEFT * 4)
            };

            if(
                board->pieces[between[0]] == MAX_PIECECODE_EMPTY &&
                board->pieces[between[1]] == MAX_PIECECODE_EMPTY &&
                board->pieces[between[2]] == MAX_PIECECODE_EMPTY &&
                board->pieces[between[3]] == (MAX_PIECECODE_ROOK | color)
            ) {
                max_movelist_add(moves, max_move_new(kingpos, between[1], MAX_MOVE_QCASTLE));
            }
        }
    }

    #undef NORMALMOVE
}


/// Generate all pseudo-legal capture moves for the side to play
void max_board_capturegen_pseudo(max_board_t *const board, max_movelist_t *const moves) {
    uint8_t side = board->ply & 1;
    max_pieces_t *state = &board->sides[side];

    max_piececode_t enemy_color = MAX_PIECECODE_BLACK >> side;

    for(max_lidx_t i = 0; i < state->pawns.len; ++i) {
        max_bpos_t pos = state->pawns.pos[i];
        max_board_pawnmovegen_loud(board, moves, enemy_color, side, pos); 
    }

    #define NORMALMOVE(from, to) do {                                       \
        max_bpos_t dest = (to);                                             \
        if(max_bpos_valid(dest)) {                                          \
            max_piececode_t piece = board->pieces[dest];                    \
            if(piece & enemy_color) {                                       \
                max_movelist_add(moves, max_move_capture((from), dest));    \
            }                                                               \
        }                                                                   \
    } while(0)

    for(max_lidx_t i = 0; i < state->knights.len; ++i) {
        max_bpos_t pos = state->knights.pos[i];
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_UP + MAX_INCREMENT_UL));

        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_DOWN + MAX_INCREMENT_DL));

        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_UR));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_RIGHT + MAX_INCREMENT_DR));


        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_UL));
        NORMALMOVE(pos, max_bpos_inc(pos, MAX_INCREMENT_LEFT + MAX_INCREMENT_DL));
    }

    for(max_lidx_t i = 0; i < state->bishops.len; ++i) {
        max_bishopgen_loud(moves, board, enemy_color, state->bishops.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->rooks.len; ++i) {
        max_rookgen_loud(moves, board, enemy_color, state->rooks.pos[i]);
    }

    for(max_lidx_t i = 0; i < state->queens.len; ++i) {
        max_rookgen_loud(moves, board, enemy_color, state->queens.pos[i]);
        max_bishopgen_loud(moves, board, enemy_color, state->queens.pos[i]);
    }
    
    if(state->king.len != 0) {
        max_bpos_t kingpos = state->king.pos[0];
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UP));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DOWN));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_LEFT));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_RIGHT));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UR));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_UL));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DR));
        NORMALMOVE(kingpos, max_bpos_inc(kingpos, MAX_INCREMENT_DL));
    }
}
