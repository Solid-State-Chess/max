#pragma once
#include "max/board.h"
#include "max/piece.h"

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
