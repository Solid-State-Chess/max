#pragma once
#include "max/def.h"
#include <stdint.h>

#ifdef MAX_DEBUG
#include <assert.h>
#endif

/// Index into a 10x12 chess board
typedef uint8_t max_square_idx_t;

typedef uint8_t max_move_flags_t;

enum {
    MAX_MOVE_EN_PASSANT = 0b00000001,
    MAX_MOVE_QCASTLE    = 0b00000010,
    MAX_MOVE_KCASTLE    = 0b00000100
};

typedef struct {
    max_square_idx_t from;
    max_square_idx_t to;
    max_move_flags_t flags;
} max_move_t;

#define MAX_MOVELIST_MAX_MOVES (50)

/// A statically-allocated list of moves to append to when searching a move tree
typedef struct {
    max_move_t moves[MAX_MOVELIST_MAX_MOVES];
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
