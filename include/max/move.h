#pragma once
#include "max/def.h"
#include "max/square.h"
#include <stdint.h>

#ifdef MAX_DEBUG
#include <assert.h>
#endif


typedef uint8_t max_move_attr_t;

enum {
    MAX_MOVE_NORMAL      = 0,
    MAX_MOVE_CAPTURE     = 1,
    MAX_MOVE_EN_PASSANT  = 2,  
    MAX_MOVE_QCASTLE     = 3,
    MAX_MOVE_KCASTLE     = 4,
    MAX_MOVE_DOUBLE      = 5,
};

/// A move with specification for 'special' moves that require additional processing
typedef struct {
    max_bidx_t from;
    max_bidx_t to;
    max_move_attr_t attr;
} max_move_t;

#define MAX_MOVELIST_LEN (256)

/// A statically-allocated list of moves to append to when searching a move tree
typedef struct {
    max_move_t moves[MAX_MOVELIST_LEN];
    uint8_t len;
} max_movelist_t;

/// Create a new, empty move list
MAX_INLINE_ALWAYS void max_movelist_new(max_movelist_t *const list) {
    list->len = 0;
}

/// Remove all moves from the given movelist
MAX_INLINE_ALWAYS void max_movelist_clear(max_movelist_t *const list) {
    list->len = 0;
}

/// Add a new move to the given move list, only bounds checked in debug
MAX_INLINE_ALWAYS void max_movelist_add(max_movelist_t *const list, max_move_t move) {
    #ifdef MAX_DEBUG
        assert(list->len + 1 <= MAX_MOVELIST_MAX_MOVES);
    #endif

    list->moves[list->len] = move;
    list->len += 1;
}

/// Create a new move with custom attributes
MAX_INLINE_ALWAYS max_move_t max_move_new(max_bidx_t from, max_bidx_t to, max_move_attr_t attr) {
    return (max_move_t){.from = from, .to = to, .attr = attr};
}

/// Create a new normal (no captures) move
MAX_INLINE_ALWAYS max_move_t max_move_normal(max_bidx_t from, max_bidx_t to) {
    return max_move_new(from, to, MAX_MOVE_NORMAL);
}

/// Create a move which captures a piece
MAX_INLINE_ALWAYS max_move_t max_move_capture(max_bidx_t from, max_bidx_t to) {
    return max_move_new(from, to, MAX_MOVE_CAPTURE);
}
